<style>
    .table td {
        text-align: left;
    }

    .table td:first-child {
        text-align: right;
        width: 230px;
    }​
</style>
# Reference

This document is a detailed and (hopefully) complete reference of the public interface of `rpclib`.
For a tutorial, take a look at the [Primer](primer.md). Also, you can find many examples in the [Cookbook](cookbook.md).



## rpc::client

```cpp
#include "rpc/client.h"
```
### Description

Implements a client that connects to a msgpack-rpc server and is able to call functions synchronously or asynchronously. This is the main interfacing point for implementing client applications. 

Use this class to connect to msgpack-rpc servers and call their exposed functions. The `client` supports calling functions synchronously and asynchronously. 

### Public functions

| | |
|---------|-------------|
|  | [client](#classrpc_1_1client_1aafbcbb90607bb189bf75a2020ee14eb8)(std::string const &addr, uint16_t port)
|  | [client](#classrpc_1_1client_1af1cf59f0da1a05982b91a2cccd1ec1ef)(client const &)=delete
|  | [~client](#classrpc_1_1client_1a87e0788aaa7a95dfc0e1b9c15dfe4163)()
| msgpack::object_handle | [call](#classrpc_1_1client_1a97f1406c306d8d6abff5f5a2e455ab72)(std::string const &func_name, Args...args)
| std::future< msgpack::object_handle > | [async_call](#classrpc_1_1client_1a786fcf533f716b1f75deed60452bb75c)(std::string const &func_name, Args...args)
| void | [send](#classrpc_1_1client_1a668d7e045da563fe4855df9c6b7a0737)(std::string const &func_name, Args...args)
| connection_state | [get_connection_state](#classrpc_1_1client_1a109a04851e70742f53549f4039de21a7)() const 
| void | [wait_all_responses](#classrpc_1_1client_1ac37437bc05b70588c079079b957eb15f)()


<h4 id="classrpc_1_1client_1aafbcbb90607bb189bf75a2020ee14eb8"> rpc::client::client(std::string const &addr, uint16_t port)</h4>




<h4 id="classrpc_1_1client_1af1cf59f0da1a05982b91a2cccd1ec1ef"> rpc::client::client(client const &)=delete</h4>




<h4 id="classrpc_1_1client_1a87e0788aaa7a95dfc0e1b9c15dfe4163"> rpc::client::~client()</h4>




<h4 id="classrpc_1_1client_1a97f1406c306d8d6abff5f5a2e455ab72">msgpack::object_handle rpc::client::call(std::string const &func_name, Args...args)</h4>

Calls a function with the given name and arguments (if any). 

##### Template parameters
`Args` The types of the arguments. 

##### Parameters
`func_name` The name of the function to call. 

`args` The arguments to pass to the function. 


<h4 id="classrpc_1_1client_1a786fcf533f716b1f75deed60452bb75c">std::future< msgpack::object_handle > rpc::client::async_call(std::string const &func_name, Args...args)</h4>

Calls a function asynchronously with the given name and arguments. 

##### Template parameters
`Args` The types of the arguments. 

##### Parameters
`func_name` The name of the function to call. 

`args` The arguments to pass to the function. 


<h4 id="classrpc_1_1client_1a668d7e045da563fe4855df9c6b7a0737">void rpc::client::send(std::string const &func_name, Args...args)</h4>

Sends a notification with the given name and arguments (if any). 

##### Template parameters
`Args` THe types of the arguments.

##### Parameters
`func_name` The name of the notification to call. 

`args` The arguments to pass to the function. 

!!! warn
    This function returns immediately (possibly before the notification is written to the socket. 

<h4 id="classrpc_1_1client_1a109a04851e70742f53549f4039de21a7">connection_state rpc::client::get_connection_state() const </h4>

Returns the current connection state. 


<h4 id="classrpc_1_1client_1ac37437bc05b70588c079079b957eb15f">void rpc::client::wait_all_responses()</h4>

Waits for the completion of all ongoing calls. 



## rpc::dispatcher

```cpp
#include "rpc/dispatcher.h"
```
### Description

This class maintains a registry of functors associated with their names, and callable using a msgpack-rpc call pack. 



### Public functions

| | |
|---------|-------------|
| void | [bind](#classrpc_1_1dispatcher_1aca9e60792934acbc3be2f2ec8589a64d)(std::string const &name, F func)
| void | [bind](#group__Tag-dispatched_1ga6e78725d4388c8a0fedd14a257cb0a42)(std::string const &name, F func, detail::tags::void_result const &, detail::tags::zero_arg const &)
| void | [bind](#group__Tag-dispatched_1gad8f621b0e8326af81522447b849f7ff9)(std::string const &name, F func, detail::tags::void_result const &, detail::tags::nonzero_arg const &)
| void | [bind](#group__Tag-dispatched_1ga95160fbf7eccec3bcfe4358382eab76b)(std::string const &name, F func, detail::tags::nonvoid_result const &, detail::tags::zero_arg const &)
| void | [bind](#group__Tag-dispatched_1ga4dc4322cf64d5b03779292551a29d36d)(std::string const &name, F func, detail::tags::nonvoid_result const &, detail::tags::nonzero_arg const &)
| void | [dispatch](#classrpc_1_1dispatcher_1a5043fec71ade41f1c2cf2f2700d387de)(msgpack::sbuffer const &msg)
| detail::response | [dispatch](#classrpc_1_1dispatcher_1a302435541506e11ee40f3d85ec97b6a4)(msgpack::object const &msg, bool suppress_exceptions=false)


<h4 id="classrpc_1_1dispatcher_1aca9e60792934acbc3be2f2ec8589a64d">void rpc::dispatcher::bind(std::string const &name, F func)</h4>

Binds a functor to a name so it becomes callable via RPC. 

##### Template parameters
`F` The type of the functor. 

##### Parameters
`name` The name of the functor. 

`func` The functor to bind. 


<h4 id="group__Tag-dispatched_1ga6e78725d4388c8a0fedd14a257cb0a42">void rpc::dispatcher::bind(std::string const &name, F func, detail::tags::void_result const &, detail::tags::zero_arg const &)</h4>

Stores a void, zero-arg functor with a name. 


<h4 id="group__Tag-dispatched_1gad8f621b0e8326af81522447b849f7ff9">void rpc::dispatcher::bind(std::string const &name, F func, detail::tags::void_result const &, detail::tags::nonzero_arg const &)</h4>

Stores a void, non-zero-arg functor with a name. 


<h4 id="group__Tag-dispatched_1ga95160fbf7eccec3bcfe4358382eab76b">void rpc::dispatcher::bind(std::string const &name, F func, detail::tags::nonvoid_result const &, detail::tags::zero_arg const &)</h4>

Stores a non-void, zero-arg functor with a name. 


<h4 id="group__Tag-dispatched_1ga4dc4322cf64d5b03779292551a29d36d">void rpc::dispatcher::bind(std::string const &name, F func, detail::tags::nonvoid_result const &, detail::tags::nonzero_arg const &)</h4>

Stores a non-void, non-zero-arg functor with a name. 


<h4 id="classrpc_1_1dispatcher_1a5043fec71ade41f1c2cf2f2700d387de">void rpc::dispatcher::dispatch(msgpack::sbuffer const &msg)</h4>

Processes a message that contains a call according to the Msgpack-RPC spec. 

##### Template parameters
##### Parameters
`msg` The buffer that contains the messagepack. 


<h4 id="classrpc_1_1dispatcher_1a302435541506e11ee40f3d85ec97b6a4">detail::response rpc::dispatcher::dispatch(msgpack::object const &msg, bool suppress_exceptions=false)</h4>

Processes a message that contains a call according to the Msgpack-RPC spec. 

##### Template parameters
##### Parameters
`msg` The messagepack object that contains the call. 

`suppress_exceptions` If true, exceptions will be caught and written as response for the client. 



## rpc::rpc_error

```cpp
#include "rpc/rpc_error.h"
```
### Description

Thrown when the server signals an error during the call. 



### Public functions

| | |
|---------|-------------|
| std::string | [get_function_name](#classrpc_1_1rpc__error_1a54f9582763c4efd63d7b85232333cd71)() const 
| msgpack::object_handle & | [get_error](#classrpc_1_1rpc__error_1a4d75a9c1717f4c2626dcf0c2bd288364)()


<h4 id="classrpc_1_1rpc__error_1a54f9582763c4efd63d7b85232333cd71">std::string rpc::rpc_error::get_function_name() const </h4>

Returns the name of the function that was called on the server. 


<h4 id="classrpc_1_1rpc__error_1a4d75a9c1717f4c2626dcf0c2bd288364">msgpack::object_handle & rpc::rpc_error::get_error()</h4>

Returns the error object that the server provided. 



## rpc::server

```cpp
#include "rpc/server.h"
```
### Description

Implements a msgpack-rpc server. This is the main interfacing point with the library for creating servers. 



### Public functions

| | |
|---------|-------------|
|  | [server](#classrpc_1_1server_1ac406b44f73cf2ff17240c0cd926a9c1e)(uint16_t port)
|  | [server](#classrpc_1_1server_1ad71ffce076d752e116cefa3672e5d188)(std::string const &address, uint16_t port)
|  | [~server](#classrpc_1_1server_1a20b9197e5ef1a22371e6fbdb7f58b330)()
| void | [run](#classrpc_1_1server_1a981d7e4a08d04d05cbac6770fab0dff8)()
| void | [async_run](#classrpc_1_1server_1a462e032fa21cad78eeacc27da103a2b7)(std::size_t worker_threads=1)
| void | [bind](#classrpc_1_1server_1a072135629430df6d5576416806f7b02c)(std::string const &name, F func)
| void | [suppress_exceptions](#classrpc_1_1server_1a95d336322c9c24cf014404a3b4f70df5)(bool suppress)
| void | [stop](#classrpc_1_1server_1a7df94a496caf38b3d679113c1f62082b)()
| void | [close_sessions](#classrpc_1_1server_1abf6bebbbeea52451aef2126d29240094)()


<h4 id="classrpc_1_1server_1ac406b44f73cf2ff17240c0cd926a9c1e"> rpc::server::server(uint16_t port)</h4>

Constructs a server that listens on the localhost on the specified port. 


<h4 id="classrpc_1_1server_1ad71ffce076d752e116cefa3672e5d188"> rpc::server::server(std::string const &address, uint16_t port)</h4>

Constructs a server that listens on the specified address on the specified port. 


<h4 id="classrpc_1_1server_1a20b9197e5ef1a22371e6fbdb7f58b330"> rpc::server::~server()</h4>




<h4 id="classrpc_1_1server_1a981d7e4a08d04d05cbac6770fab0dff8">void rpc::server::run()</h4>

Starts the server loop. This is a blocking call. 


<h4 id="classrpc_1_1server_1a462e032fa21cad78eeacc27da103a2b7">void rpc::server::async_run(std::size_t worker_threads=1)</h4>

Starts the server loop on a separate thread. This is a non-blocking call. 

##### Template parameters
##### Parameters
`worker_threads` The number of worker threads to start. 


<h4 id="classrpc_1_1server_1a072135629430df6d5576416806f7b02c">void rpc::server::bind(std::string const &name, F func)</h4>

Binds a functor to a name so it becomes callable via RPC. 

##### Template parameters
`F` The type of the functor. 

##### Parameters
`name` The name of the functor. 

`func` The functor to bind. 


<h4 id="classrpc_1_1server_1a95d336322c9c24cf014404a3b4f70df5">void rpc::server::suppress_exceptions(bool suppress)</h4>

Sets the exception behavior in handlers. By default, handlers throwing will crash the server. If suppressing is on, the server will try to gather textual data and return it to the client as an error response. 

!!! warn
    Setting this flag only affects subsequent connections. 

<h4 id="classrpc_1_1server_1a7df94a496caf38b3d679113c1f62082b">void rpc::server::stop()</h4>

Stops the server. 

!!! warn
    This should not be called from worker threads. 

<h4 id="classrpc_1_1server_1abf6bebbbeea52451aef2126d29240094">void rpc::server::close_sessions()</h4>

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


<h4 id="classrpc_1_1this__handler__t_1a6cfb57fa89c28bd49c6ec82c42c32e87">void rpc::this_handler_t::respond_error(T &&err_obj)</h4>

Sets an arbitrary object to be sent back as an error response to the client. 

##### Template parameters
`T` The type of the error object. 

##### Parameters
`err_obj` The error object. This can be anything that is possible to encode with messagepack (even custom structures). 


<h4 id="classrpc_1_1this__handler__t_1ab825539a615b55772ebc1513cf2b44c7">void rpc::this_handler_t::respond(T &&resp_obj)</h4>

Sets an arbitrary object to be sent back as the response to the call. 

##### Template parameters
`T` The type of the response object. 

##### Parameters
`resp_obj` The response object. This can be anything that is possible to encode with messagepack (even custom structures). 

!!! warn
    The normal return value of the function (if any) will be ignored if a special response is set. 

<h4 id="classrpc_1_1this__handler__t_1a179997ea536a93f1b8054e0a480876a5">void rpc::this_handler_t::disable_response()</h4>

Instructs the server to not send a response to the client (ignoring any errors and return values). 

!!! warn
    It is unusual to not send a response to requests, and doing so might cause problems in the client (depending on its implementation). 

<h4 id="classrpc_1_1this__handler__t_1aac9c17130dea7ded4bf020a370a0b9aa">void rpc::this_handler_t::enable_response()</h4>

Enables sending a response to the call. Sending the response is by default enabled. Enabling the response multiple times have no effect. 


<h4 id="classrpc_1_1this__handler__t_1a0a53e27b4d8d5b542790b218029d26f4">void rpc::this_handler_t::clear()</h4>

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


<h4 id="classrpc_1_1this__server__t_1a7e522ccc489d7376e2d795f030f3890f">void rpc::this_server_t::stop()</h4>

Gracefully stops the server. 


<h4 id="classrpc_1_1this__server__t_1a127035c6f2281a5a1bfadf19f4dfe451">void rpc::this_server_t::cancel_stop()</h4>

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


<h4 id="classrpc_1_1this__session__t_1ae0551bc44674bdd43e33a4d4a36781b0">void rpc::this_session_t::post_exit()</h4>

Gracefully exits the session (i.e. ongoing writes and reads are completed; queued writes and reads are not). 

!!! warn
    Use this function if you need to close the connection from a handler. 


