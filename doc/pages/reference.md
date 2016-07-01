# Reference

## rpc::client

Implements a client that connects to a msgpack-rpc server and is able to call functions synchronously or asynchronously. This is the main interfacing point for implementing client applications. 

Use this class to connect to msgpack-rpc servers and call their exposed functions. The `client` supports calling functions synchronously and asynchronously. 

## rpc::dispatcher

This class maintains a registry of functors associated with their names, and callable using a msgpack-rpc call pack. 



## rpc::rpc_error

Thrown when the server signals an error during the call. 



## rpc::server

Implements a msgpack-rpc server. This is the main interfacing point with the library for creating servers. 



## rpc::this_handler_t

Encapsulates information about the currently executing handler. This is the interface through which bound functions may return errors, arbitrary type responses or prohibit sending a response. 

None

## rpc::this_server_t

Allows controlling the server instance from the currently executing handler. 



## rpc::this_session_t

Encapsulates information about the server session/connection this handler is running in. This is the interface through which bound functions may interact with the session. 

None

