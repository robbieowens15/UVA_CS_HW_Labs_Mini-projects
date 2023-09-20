from concurrent import futures
from pickle import NONE
import grpc
import logging
import twophase_pb2
import twophase_pb2_grpc

logger = logging.getLogger(__name__)

class MyCoordinator(twophase_pb2_grpc.CoordinatorServicer):
    def __init__(self, log, worker_stubs):
        self._log = log
        self._worker_stubs = worker_stubs
        self._counter = 0
        self._new_value = None
        self._cur_transaction_id = -1
        self._state = "INIT"

        #Recover
        log_entry = self._log.get_last_entry()
        if log_entry == None:
            return #New log
        elif log_entry['state'] == "INIT":
            self._counter = log_entry['counter']
            return #Wait for request
        elif log_entry['state'] == "WAITING":
            self._state = "WAITING"
            self._counter = log_entry['counter']
            self._cur_transaction_id = log_entry['transID']
            self._new_value = log_entry['new_value']
        elif log_entry['state'] == "COMMIT":
            self._state = "COMMIT"
            self._new_value = log_entry['new_value']
            self._counter = log_entry['counter']
            self._cur_transaction_id = log_entry['transID']
        elif log_entry['state'] == "ABORT":
            self._state = "ABORT"
            self._counter = log_entry['counter']
            self._cur_transaction_id = log_entry['transID']

        self.Brain() #Enter Two Phase Commit


    def HandleRequest(self, request):
        #Preprocessing
        self._counter += 1
        self._new_value = request.content
        self._cur_transaction_id = self._counter

        #Backup
        self._log.set_last_entry({
            'new_value': self._new_value,
            'counter': self._counter,
            'transID': self._cur_transaction_id,
            'state': "WAITING"
        })
        self._state = "WAITING"

        return False
    
    def ConductElection(self):
        #Collect Votes
        commit = True
        c = 0
        for worker_stub in self._worker_stubs:
            ballot = worker_stub.CastVote(twophase_pb2.Ballot(
                content = self._new_value,
                transID = int(self._cur_transaction_id),
            ))
            if ballot.content == "ABORT" and ballot.transID == self._cur_transaction_id:
                commit = False
            elif ballot.content == "COMMIT" and ballot.transID == self._cur_transaction_id:
                c += 1
        
        if not c == len(self._worker_stubs):
            commit = False

        #Backup
        next_state = "COMMIT" if commit else "ABORT"
        self._log.set_last_entry({
            'new_value': self._new_value,
            'counter': self._counter,
            'transID': self._cur_transaction_id,
            'state': next_state
        })
        self._state = next_state

        return False

    def ResolveElection(self):
        #Pass Down Decision
        if self._state == "ABORT":
            #Make old value available
            for worker_stub in self._worker_stubs:
                worker_stub.Abort(twophase_pb2.Ballot(
                    content = self._new_value,
                    transID = int(self._cur_transaction_id)
                ))
        else:
            #Change Value and make available
            for worker_stub in self._worker_stubs:
                worker_stub.SetValue(twophase_pb2.Ballot(
                    content = self._new_value,
                    transID = int(self._cur_transaction_id)
                ))
        #Log
        self._log.set_last_entry({
            'counter': self._counter,
            'state': "INIT",
        })
        self._state = "INIT"

        return True

    def Brain(self, request=None):
        done = False
        while done == False:
            if self._state == "INIT" and not request == None:
                self.HandleRequest(request)
                done = False
            elif self._state == "WAITING":
                self.ConductElection()
                done = False
            elif self._state == "COMMIT" or self._state == "ABORT": 
                self.ResolveElection()
                done = True
            else:
                done = True

    def SetValue(self, request, context):
        if not self._state == "INIT":
            #Chose to abort if processing a value
            return twophase_pb2.Empty()

        self.Brain(request)
        return twophase_pb2.Empty()


def create_coordinator(coordinator_log, worker_stubs, **extra_args):
    server = grpc.server(futures.ThreadPoolExecutor(max_workers=1, **extra_args))
    twophase_pb2_grpc.add_CoordinatorServicer_to_server(MyCoordinator(coordinator_log, worker_stubs), server)
    return server


if __name__ == '__main__':
    import argparse
    import os
    import persistent_log
    import time
    parser = argparse.ArgumentParser(
        description='Run the coordinator server.'
    )
    parser.add_argument('server_address')
    parser.add_argument('log_file')
    parser.add_argument('worker_addresses', nargs='+')
    args = parser.parse_args()
    log = persistent_log.FilePersistentLog(args.log_file)
    worker_stubs = [
        twophase_pb2_grpc.WorkerStub(grpc.insecure_channel(address)) \
        for address in args.worker_addresses
    ]
    server = create_coordinator(log, worker_stubs)
    server.add_insecure_port(args.server_address)
    server.start()
    while True:
        time.sleep(3600)
