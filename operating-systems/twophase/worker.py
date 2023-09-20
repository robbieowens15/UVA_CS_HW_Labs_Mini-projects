from concurrent import futures
import logging
from pickle import NONE
import grpc
import twophase_pb2
import twophase_pb2_grpc

logger = logging.getLogger(__name__)
class MyWorker(twophase_pb2_grpc.WorkerServicer):
    def __init__(self, log):
        self._log = log
        self._value = None
        self._next_value = None
        self._state = "INIT"
        self._transaction_id = -1

        #Reserection Recovery Code
        log_entry = self._log.get_last_entry()
        if log_entry == None:
            return 
        elif log_entry['state'] == "INIT":
            self._value = log_entry['value']
            self._transaction_id = log_entry['transID']
        elif log_entry['state'] == "AGREED-TO-COMMIT":
            self._value = log_entry['value']
            self._next_value = log_entry['new_value']
            self._transaction_id = log_entry['transID']
            self._state = "AGREED-TO-COMMIT"


    #Worker "Commit" phase
    def SetValue(self, request, context):
        if self._next_value == None:
            #COMMIT Already finished
            return twophase_pb2.Empty()

        self.Commit()
        
        return twophase_pb2.Empty()

    def GetCommitted(self, request, context):
        if self._state == "INIT":
            return twophase_pb2.MaybeValue(
                available=True,
                content=self._value,
            )
        else:
            return twophase_pb2.MaybeValue(
                available=False,
                content="",
            )
    
    #first phase for worker in two phase commit
    def CastVote(self, request, context):
        #If working on a commit do not vote on a new one
        if not self._state == "INIT" or request.transID < self._transaction_id:
            
            return twophase_pb2.Ballot(
                content="ABORT",
                transID=int(self._transaction_id)
            )

        #WORKER CHOSING TO COMMIT
        self._next_value = request.content
        self._transaction_id = request.transID

        self._log.set_last_entry({
            'value': self._value,
            'new_value': self._next_value,
            'state': "AGREED-TO-COMMIT",
            'transID': self._transaction_id,
        })

        self._state = "AGREED-TO-COMMIT"

        return twophase_pb2.Ballot(
            content="COMMIT",
            transID=int(self._transaction_id)
        )
    
    def Commit(self):
        #Reset working for next request
        self._value = self._next_value
        self._next_value = None

        self._log.set_last_entry({
            'value': self._value,
            'state': "INIT",
            'transID': self._transaction_id,
        })

        self._state = "INIT"
    
    def Abort(self, request, context):
        self._next_value = None
        next_state = "INIT" if request.transID == self._transaction_id else "AGREED-TO-COMMIT"
        self._log.set_last_entry({
            'value': self._value,
            'state': next_state,
            'transID': self._transaction_id,
        })

        self._state = "INIT"

        return twophase_pb2.Empty()
    

def create_worker(worker_log, **extra_args):
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=1), **extra_args)
    twophase_pb2_grpc.add_WorkerServicer_to_server(MyWorker(worker_log), server)
    return server

if __name__ == '__main__':
    import argparse
    import os
    import persistent_log
    import time
    parser = argparse.ArgumentParser(
        description='Run a worker'
    )
    parser.add_argument('server_address')
    parser.add_argument('log_file')
    args = parser.parse_args()
    log = persistent_log.FilePersistentLog(args.log_file)
    server = create_worker(log)
    server.add_insecure_port(args.server_address)
    server.start()
    while True:
        time.sleep(3600)
