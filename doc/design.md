# rpclib 3.0 design

rpclib intends to be a generic rpc server, allowing arbitrary transports and
protocols.

## Concepts

  * Server: a class that implements server logic, i.e. the actual interaction
    with the I/O APIs.
  * Call: a function name and a list of parameters.
  * Serializer/Deserializer: a class that encodes or decodes a call to/from
    data
  * Transport: an abstract notion of a communication layer where serialized
    calls can be written and read from. Think TCP, UDP, HTTP etc.
  * Protocol: a set of rules that describe how a given call is sent and how
    its result is returned.
  * Dispatcher: given a deserialized call, perform the call, serialize the
    result and write back to the transport using the protocol.
