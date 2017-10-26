<style>
    .table td {
        text-align: left;
    }

    .table td:first-child {
        text-align: right;
        width: 230px;
    }​
</style>
This document is a detailed and (hopefully) complete reference of the public interface of `rpclib`.
For a tutorial, take a look at the [Primer](primer.md). Also, you can find many examples in the [Cookbook](cookbook.md).



## rpc::client

```cpp
#include "rpc/client.h"
```
### Description

Implements a client that connects to a msgpack-rpc server and is able to call functions synchronously or asynchronously. This is the main interfacing point for implementing client applications. 

Use this class to connect to msgpack-rpc servers and call their exposed functions. This class supports calling functions synchronously and asynchronously. When the client object is created, it initiates connecting to the given server asynchronically and disconnects when it is destroyed. 

### Public functions

| | |
|---------|-------------|
|  | [client](#classrpc_1_1client_1aafbcbb90607bb189bf75a2020ee14eb8)(std::string const &addr, uint16_t port)
|  | [~client](#classrpc_1_1client_1a87e0788aaa7a95dfc0e1b9c15dfe4163)()
| RPCLIB_MSGPACK::object_handle | [call](#classrpc_1_1client_1aedc166b5a80820be198ef134f522b049)(std::string const &func_name, Args... args)
| std::future< RPCLIB_MSGPACK::object_handle > | [async_call](#classrpc_1_1client_1a2e3702a314c8c0a00bfac652b82d16cc)(std::string const &func_name, Args... args)
| void | [send](#classrpc_1_1client_1a5f5ad1d1d0630178a51ae219cd831b44)(std::string const &func_name, Args... args)
| nonstd::optional< int64_t > | [get_timeout](#classrpc_1_1client_1a2c264af4d7169574452b9f968ffde87d)() const
| void | [set_timeout](#classrpc_1_1client_1af890e3067745861642e2ba1a65bebce6)(int64_t value)
| void | [clear_timeout](#classrpc_1_1client_1a89eeffaf87bf0470a65c9c8ca40562bb)()
| connection_state | [get_connection_state](#classrpc_1_1client_1a710037bce0d9b80127a98eb6cd54caf1)() const
| void | [wait_all_responses](#classrpc_1_1client_1ac37437bc05b70588c079079b957eb15f)()


<h4 id="classrpc_1_1client_1aafbcbb90607bb189bf75a2020ee14eb8" class="doxy">rpc::client::client</h4>
```cpp
 rpc::client::client(std::string const &addr, uint16_t port);
```

Constructs a client. 

##### Parameters
`addr` The address of the server to connect to. This might be an IP address or a host name, too. 

`port` The port on the server to connect to. 

##### Details
When a client is constructed, it initiates a connection asynchronically. This means that it will not block while the connection is established. However, when the first call is performed, it *might* block if the connection was not already established.


<h4 id="classrpc_1_1client_1a87e0788aaa7a95dfc0e1b9c15dfe4163" class="doxy">rpc::client::~client</h4>
```cpp
 rpc::client::~client();
```

Destructor. 

##### Details
During destruction, the connection to the server is gracefully closed. This means that any outstanding reads and writes are completed first. 


<h4 id="classrpc_1_1client_1aedc166b5a80820be198ef134f522b049" class="doxy">rpc::client::call</h4>
```cpp
RPCLIB_MSGPACK::object_handle rpc::client::call(std::string const &func_name, Args... args);
```

Calls a function with the given name and arguments (if any). 

##### Template parameters
`Args` The types of the arguments. Each type in this parameter pack have to be serializable by msgpack.

##### Parameters
`func_name` The name of the function to call on the server. 

`args` A variable number of arguments to pass to the called function.

##### Return value
A RPCLIB_MSGPACK::object containing the result of the function (if any). To obtain a typed value, use the msgpack API.


<h4 id="classrpc_1_1client_1a2e3702a314c8c0a00bfac652b82d16cc" class="doxy">rpc::client::async_call</h4>
```cpp
std::future< RPCLIB_MSGPACK::object_handle > rpc::client::async_call(std::string const &func_name, Args... args);
```

Calls a function asynchronously with the given name and arguments. 

##### Template parameters
`Args` The types of the arguments.

##### Parameters
`func_name` The name of the function to call. 

`args` The arguments to pass to the function.

##### Details
A call is performed asynchronously in the context of the client, i.e. this is not to be confused with parallel execution on the server. This function differs from `call` in that it does not wait for the result of the function. Instead, it returns a std::future that can be used to retrieve the result later.

##### Return value
A std::future, possibly holding a future result (which is a RPCLIB_MSGPACK::object). 


<h4 id="classrpc_1_1client_1a5f5ad1d1d0630178a51ae219cd831b44" class="doxy">rpc::client::send</h4>
```cpp
void rpc::client::send(std::string const &func_name, Args... args);
```

Sends a notification with the given name and arguments (if any). 

##### Template parameters
`Args` THe types of the arguments.

##### Parameters
`func_name` The name of the notification to call. 

`args` The arguments to pass to the function. 

##### Details
Notifications are a special kind of calls. They can be used to notify the server, while not expecting a response. In `rpclib` terminology, a notification is like an `async_call` without a return value.

!!! warn
    This function returns immediately (possibly before the notification is written to the socket).

<h4 id="classrpc_1_1client_1a2c264af4d7169574452b9f968ffde87d" class="doxy">rpc::client::get_timeout</h4>
```cpp
nonstd::optional< int64_t > rpc::client::get_timeout() const;
```

Returns the timeout setting of this client in milliseconds. 

##### Details
The timeout is applied to synchronous calls. If the timeout expires without receiving a response from the server, 

!!! warn
    The timeout has no effect on async calls. For those, the preferred timeout mechanism remains using std::future.

<h4 id="classrpc_1_1client_1af890e3067745861642e2ba1a65bebce6" class="doxy">rpc::client::set_timeout</h4>
```cpp
void rpc::client::set_timeout(int64_t value);
```

Sets the timeout for synchronous calls. For more information, see 


<h4 id="classrpc_1_1client_1a89eeffaf87bf0470a65c9c8ca40562bb" class="doxy">rpc::client::clear_timeout</h4>
```cpp
void rpc::client::clear_timeout();
```

Clears the timeout for synchronous calls. For more information, see 


<h4 id="classrpc_1_1client_1a710037bce0d9b80127a98eb6cd54caf1" class="doxy">rpc::client::get_connection_state</h4>
```cpp
connection_state rpc::client::get_connection_state() const;
```

Returns the current connection state. 


<h4 id="classrpc_1_1client_1ac37437bc05b70588c079079b957eb15f" class="doxy">rpc::client::wait_all_responses</h4>
```cpp
void rpc::client::wait_all_responses();
```

Waits for the completion of all ongoing calls. 



## rpc::rpc_error

```cpp
#include "rpc/rpc_error.h"
```
### Description

This exception is thrown by the client when the server signals an error during a call. 

This type allows clients to handle arbitrary error objects as the msgpack-rpc specification allows. In client code you probably don't want to throw it, hence its constructor is private. 

### Public functions

| | |
|---------|-------------|
| std::string | [get_function_name](#classrpc_1_1rpc__error_1ac45388bcde0a436b888c907015df01e2)() const
| RPCLIB_MSGPACK::object_handle & | [get_error](#classrpc_1_1rpc__error_1a88ab8f211393ae62813042a797c08663)()


<h4 id="classrpc_1_1rpc__error_1ac45388bcde0a436b888c907015df01e2" class="doxy">rpc::rpc_error::get_function_name</h4>
```cpp
std::string rpc::rpc_error::get_function_name() const;
```

Returns the name of the function that was called on the server while the error occurred. 


<h4 id="classrpc_1_1rpc__error_1a88ab8f211393ae62813042a797c08663" class="doxy">rpc::rpc_error::get_error</h4>
```cpp
RPCLIB_MSGPACK::object_handle & rpc::rpc_error::get_error();
```

Returns the error object that the server provided. 



## rpc::server

```cpp
#include "rpc/server.h"
```
### Description

Implements a msgpack-rpc server. This is the main interfacing point with the library for creating servers. 

The server maintains a registry of function bindings that it uses to dispatch calls. It also takes care of managing worker threads and TCP connections. The server does not start listening right after construction in order to allow binding functions before that. Use the `run` or `async_run` functions to start listening on the port. This class is not copyable, but moveable. 

### Public functions

| | |
|---------|-------------|
|  | [server](#classrpc_1_1server_1ac406b44f73cf2ff17240c0cd926a9c1e)(uint16_t port)
|  | [server](#classrpc_1_1server_1a744b961d3b151b3449a4e0da37bd471a)(server &&other) noexcept
|  | [server](#classrpc_1_1server_1ad71ffce076d752e116cefa3672e5d188)(std::string const &address, uint16_t port)
|  | [~server](#classrpc_1_1server_1a20b9197e5ef1a22371e6fbdb7f58b330)()
| server | [operator=](#classrpc_1_1server_1ac3cf4848fc3969cd26ba5e3bd2dc411b)(server &&other)
| void | [run](#classrpc_1_1server_1a981d7e4a08d04d05cbac6770fab0dff8)()
| void | [async_run](#classrpc_1_1server_1a462e032fa21cad78eeacc27da103a2b7)(std::size_t worker_threads=1)
| void | [bind](#classrpc_1_1server_1a072135629430df6d5576416806f7b02c)(std::string const &name, F func)
| void | [suppress_exceptions](#classrpc_1_1server_1a95d336322c9c24cf014404a3b4f70df5)(bool suppress)
| void | [stop](#classrpc_1_1server_1a7df94a496caf38b3d679113c1f62082b)()
| void | [close_sessions](#classrpc_1_1server_1abf6bebbbeea52451aef2126d29240094)()


<h4 id="classrpc_1_1server_1ac406b44f73cf2ff17240c0cd926a9c1e" class="doxy">rpc::server::server</h4>
```cpp
 rpc::server::server(uint16_t port);
```

Constructs a server that listens on the localhost on the specified port. 

##### Parameters
`port` The port number to listen on. 


<h4 id="classrpc_1_1server_1a744b961d3b151b3449a4e0da37bd471a" class="doxy">rpc::server::server</h4>
```cpp
 rpc::server::server(server &&other) noexcept;
```

Move constructor. This is implemented by calling the move assignment operator. 

##### Parameters
`other` The other instance to move from. 


<h4 id="classrpc_1_1server_1ad71ffce076d752e116cefa3672e5d188" class="doxy">rpc::server::server</h4>
```cpp
 rpc::server::server(std::string const &address, uint16_t port);
```

Constructs a server that listens on the specified address on the specified port. 

##### Parameters
`address` The address to bind to. This only works if oee of your network adapaters control the given address. 

`port` The port number to listen on. 


<h4 id="classrpc_1_1server_1a20b9197e5ef1a22371e6fbdb7f58b330" class="doxy">rpc::server::~server</h4>
```cpp
 rpc::server::~server();
```

Destructor. 

##### Details
When the server is destroyed, all ongoin sessions are closed gracefully. 


<h4 id="classrpc_1_1server_1ac3cf4848fc3969cd26ba5e3bd2dc411b" class="doxy">rpc::server::operator=</h4>
```cpp
server rpc::server::operator=(server &&other);
```

Move assignment operator. 

##### Parameters
`other` The other instance to move from. 

##### Return value
The result of the assignment. 


<h4 id="classrpc_1_1server_1a981d7e4a08d04d05cbac6770fab0dff8" class="doxy">rpc::server::run</h4>
```cpp
void rpc::server::run();
```

Starts the server loop. This is a blocking call. 

##### Details
First and foremost, running the event loop causes the server to start listening on the specified port. Also, as connections are established and calls are made by clients, the server executes the calls as part of this call. This means that the handlers are executed on the thread that calls `run`. Reads and writes are initiated by this function internally as well. 


<h4 id="classrpc_1_1server_1a462e032fa21cad78eeacc27da103a2b7" class="doxy">rpc::server::async_run</h4>
```cpp
void rpc::server::async_run(std::size_t worker_threads=1);
```

Starts the server loop on one or more threads. This is a non-blocking call. 

##### Parameters
`worker_threads` The number of worker threads to start. 

##### Details
This function behaves similarly to `run`, except the event loop is optionally started on different threads. Effectively this sets up a worker thread pool for the server. Handlers will be executed on one of the threads.


<h4 id="classrpc_1_1server_1a072135629430df6d5576416806f7b02c" class="doxy">rpc::server::bind</h4>
```cpp
void rpc::server::bind(std::string const &name, F func);
```

Binds a functor to a name so it becomes callable via RPC. 

##### Template parameters
`F` The type of the functor. 

##### Parameters
`name` The name of the functor. 

`func` The functor to bind. 

##### Details
This function template accepts a wide range of callables. The arguments and return types of these callables should be serializable by msgpack. `bind` effectively generates a suitable, light-weight compile-time wrapper for the functor.


<h4 id="classrpc_1_1server_1a95d336322c9c24cf014404a3b4f70df5" class="doxy">rpc::server::suppress_exceptions</h4>
```cpp
void rpc::server::suppress_exceptions(bool suppress);
```

Sets the exception behavior in handlers. By default, handlers throwing will crash the server. If suppressing is on, the server will try to gather textual data and return it to the client as an error response. 

!!! warn
    Setting this flag only affects subsequent connections. 

<h4 id="classrpc_1_1server_1a7df94a496caf38b3d679113c1f62082b" class="doxy">rpc::server::stop</h4>
```cpp
void rpc::server::stop();
```

Stops the server. 

!!! warn
    This should not be called from worker threads. 

<h4 id="classrpc_1_1server_1abf6bebbbeea52451aef2126d29240094" class="doxy">rpc::server::close_sessions</h4>
```cpp
void rpc::server::close_sessions();
```

Closes all sessions gracefully. 



## rpc::this_handler_t

```cpp
#include "rpc/this_handler.h"
```
### Description

Encapsulates information about the currently executing handler. This is the interface through which bound functions may return errors, arbitrary type responses or prohibit sending a response. 



### Public functions

| | |
|---------|-------------|
| void | [respond_error](#classrpc_1_1this__handler__t_1a6cfb57fa89c28bd49c6ec82c42c32e87)(T &&err_obj)
| void | [respond](#classrpc_1_1this__handler__t_1ab825539a615b55772ebc1513cf2b44c7)(T &&resp_obj)
| void | [disable_response](#classrpc_1_1this__handler__t_1a179997ea536a93f1b8054e0a480876a5)()
| void | [enable_response](#classrpc_1_1this__handler__t_1aac9c17130dea7ded4bf020a370a0b9aa)()
| void | [clear](#classrpc_1_1this__handler__t_1a0a53e27b4d8d5b542790b218029d26f4)()


<h4 id="classrpc_1_1this__handler__t_1a6cfb57fa89c28bd49c6ec82c42c32e87" class="doxy">rpc::this_handler_t::respond_error</h4>
```cpp
void rpc::this_handler_t::respond_error(T &&err_obj);
```

Sets an arbitrary object to be sent back as an error response to the client. 

##### Template parameters
`T` The type of the error object. 

##### Parameters
`err_obj` The error object. This can be anything that is possible to encode with messagepack (even custom structures). 


<h4 id="classrpc_1_1this__handler__t_1ab825539a615b55772ebc1513cf2b44c7" class="doxy">rpc::this_handler_t::respond</h4>
```cpp
void rpc::this_handler_t::respond(T &&resp_obj);
```

Sets an arbitrary object to be sent back as the response to the call. 

##### Template parameters
`T` The type of the response object. 

##### Parameters
`resp_obj` The response object. This can be anything that is possible to encode with messagepack (even custom structures). 

!!! warn
    The normal return value of the function (if any) will be ignored if a special response is set. 

<h4 id="classrpc_1_1this__handler__t_1a179997ea536a93f1b8054e0a480876a5" class="doxy">rpc::this_handler_t::disable_response</h4>
```cpp
void rpc::this_handler_t::disable_response();
```

Instructs the server to not send a response to the client (ignoring any errors and return values). 

!!! warn
    It is unusual to not send a response to requests, and doing so might cause problems in the client (depending on its implementation). 

<h4 id="classrpc_1_1this__handler__t_1aac9c17130dea7ded4bf020a370a0b9aa" class="doxy">rpc::this_handler_t::enable_response</h4>
```cpp
void rpc::this_handler_t::enable_response();
```

Enables sending a response to the call. Sending the response is by default enabled. Enabling the response multiple times have no effect. 


<h4 id="classrpc_1_1this__handler__t_1a0a53e27b4d8d5b542790b218029d26f4" class="doxy">rpc::this_handler_t::clear</h4>
```cpp
void rpc::this_handler_t::clear();
```

Sets all state of the object to default. 



## rpc::this_server_t

```cpp
#include "rpc/this_server.h"
```
### Description

Allows controlling the server instance from the currently executing handler. 



### Public functions

| | |
|---------|-------------|
| void | [stop](#classrpc_1_1this__server__t_1a7e522ccc489d7376e2d795f030f3890f)()
| void | [cancel_stop](#classrpc_1_1this__server__t_1a127035c6f2281a5a1bfadf19f4dfe451)()


<h4 id="classrpc_1_1this__server__t_1a7e522ccc489d7376e2d795f030f3890f" class="doxy">rpc::this_server_t::stop</h4>
```cpp
void rpc::this_server_t::stop();
```

Gracefully stops the server. 


<h4 id="classrpc_1_1this__server__t_1a127035c6f2281a5a1bfadf19f4dfe451" class="doxy">rpc::this_server_t::cancel_stop</h4>
```cpp
void rpc::this_server_t::cancel_stop();
```

Cancels a requested stop operation. 



## rpc::this_session_t

```cpp
#include "rpc/this_session.h"
```
### Description

Encapsulates information about the server session/connection this handler is running in. This is the interface through which bound functions may interact with the session. 



### Public functions

| | |
|---------|-------------|
| void | [post_exit](#classrpc_1_1this__session__t_1ae0551bc44674bdd43e33a4d4a36781b0)()
| session_id_t | [id](#classrpc_1_1this__session__t_1aada2250ec9dd3d88781ca16eb4352047)() const


<h4 id="classrpc_1_1this__session__t_1ae0551bc44674bdd43e33a4d4a36781b0" class="doxy">rpc::this_session_t::post_exit</h4>
```cpp
void rpc::this_session_t::post_exit();
```

Gracefully exits the session (i.e. ongoing writes and reads are completed; queued writes and reads are not). 

!!! warn
    Use this function if you need to close the connection from a handler. 

<h4 id="classrpc_1_1this__session__t_1aada2250ec9dd3d88781ca16eb4352047" class="doxy">rpc::this_session_t::id</h4>
```cpp
session_id_t rpc::this_session_t::id() const;
```

Returns an ID that uniquely identifies a session. 

!!! warn
    This is not an ID for the client. If the client disconnects and reconnects, this ID may change. That being said, you can use this ID to store client-specific information *for the duration of the session. 


## rpc::timeout

```cpp
#include "rpc/rpc_error.h"
```
### Description

This exception is thrown by the client when either the connection or a call takes more time than it is set in set_timeout. 



### Public functions

| | |
|---------|-------------|
| const char * | [what](#classrpc_1_1timeout_1ad782f083798c650188b5927a226c3b04)() const noexcept override


<h4 id="classrpc_1_1timeout_1ad782f083798c650188b5927a226c3b04" class="doxy">rpc::timeout::what</h4>
```cpp
const char * rpc::timeout::what() const noexcept override;
```

Describes the exception. 



