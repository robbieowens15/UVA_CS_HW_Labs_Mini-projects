# Generated by the gRPC Python protocol compiler plugin. DO NOT EDIT!
import grpc

import test_util_pb2 as test__util__pb2


class TestUtilPingStub(object):
  # missing associated documentation comment in .proto file
  pass

  def __init__(self, channel):
    """Constructor.

    Args:
      channel: A grpc.Channel.
    """
    self.Ping = channel.unary_unary(
        '/TestUtilPing/Ping',
        request_serializer=test__util__pb2.TestUtilEmpty.SerializeToString,
        response_deserializer=test__util__pb2.TestUtilEmpty.FromString,
        )


class TestUtilPingServicer(object):
  # missing associated documentation comment in .proto file
  pass

  def Ping(self, request, context):
    # missing associated documentation comment in .proto file
    pass
    context.set_code(grpc.StatusCode.UNIMPLEMENTED)
    context.set_details('Method not implemented!')
    raise NotImplementedError('Method not implemented!')


def add_TestUtilPingServicer_to_server(servicer, server):
  rpc_method_handlers = {
      'Ping': grpc.unary_unary_rpc_method_handler(
          servicer.Ping,
          request_deserializer=test__util__pb2.TestUtilEmpty.FromString,
          response_serializer=test__util__pb2.TestUtilEmpty.SerializeToString,
      ),
  }
  generic_handler = grpc.method_handlers_generic_handler(
      'TestUtilPing', rpc_method_handlers)
  server.add_generic_rpc_handlers((generic_handler,))
