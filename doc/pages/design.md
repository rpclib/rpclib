# Design

This chapter describes the internal design of `rpclib` and provides some insight into the
engineering tradeoffs considered.

## Dependencies of `rpclib`

`rpclib` is self-contained, but it does use third party code. These are the following libraries:

  * **asio** (used for networking and async capabilities)
  * **fmtlib** (used for string formatting in log and exception messages)
  * **msgpack** (used for encoding and decoding the protocol)

These dependencies are stored inside the repository of `rpclib`, but they are hidden both during
compilation and linking. This is achieved by using the pimpl pattern and changing the names of the
namespaces in their source files (apart from `msgpack`, none of the dependencies are visible in the
headers even).

This means that as a user, you don't have to worry about linker problems if you integrate `rpclib`
into your project; and you don't need gather its dependencies. This reduces friction. The tradeoff
is that the size of your binary will increase if you use one of these dependencies in your project
outside `rpclib`.

!!! info
    **How can I compile `rpclib` using dependencies outside its repository?** While not officially
    supported, it's possible that you will want to link to a system-installed `asio` because you
    are using it in your application anyway and want to avoid code bloat. To do this, delete the
    library from the dependencies subfolder of the repository, and define `RPCLIB_ASIO` as `asio`
    (or `boost::asio`). This will cause `rpclib` to find the system-wide installed `asio` and use
    the namespace name provided. You might also need to change some of the preprocessor definitions
    in the CMakeLists.txt if you want to use the boost-flavored asio, not the standalone one.

## The design of the server

### The dispatcher

`rpclib` maintains a registry of exposed functions in a `dispatcher`. The dispatcher is a template
class and this is the part which pulls in most of the template metaprogramming in the library. The primary purpose of the metaprogramming is to generate wrappers that can manage calling an arbitrary functor from a msgpack-encoded message; then encode the result of the function (if any) in msgpack.

The generated wrappers have a uniform signature (`dispatcher::adaptor_type`) which allows storing
them in map. The dispatching is performed by looking up the right functor by name.

### The server loop

The call to `server::run` starts an `asio`-loop. Everything that the server does is performed in
this loop. This includes not only executing the handlers, but also parsing the input and writing
the output. `async_run` will spawn multiple worker threads that all `run` the loop. Thanks to the
great design of `asio`, this makes them act like a thread pool, i.e. waiting in line to take the
next available work item. This scales pretty well for networked applications.

## The design of the client

TBD

## How and why the pimpl pattern is used

`rpclib` uses a variant of the [fast pimpl idiom](http://www.gotw.ca/gotw/028.htm). The reason for
this is that one of the goals of the library is to provide a dependable rpc solution for projects
and make an effort to be easily upgrade-able when new versions come out. This is also one of the
reasons why the library is not header-only.

Instead of a `unique_ptr` for the pimpl pointer, the library uses a pointer-like class which stores
its data in a `std::aligned_storage`. This increases the data locality during the calls and reduces
dynamic allocation. The tradeoff is that the size of the storage is fixed, so adding extra data in an update is only possible with some bounds (the sizes used are a bit bigger then needed, so there is some room to do this without breaking binary compatibility).

# Where to go from here

As a user, there isn't much else to learn about this library. However, if you are interested, you may
want to check out the [contribution guidelines](contributing.md), the [issue tracker](https://waffle.io/rpclib/rpclib), and [roadmap](roadmap.md) and start hacking on `rpclib`!

