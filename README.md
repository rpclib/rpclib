
# callme [![Build Status](https://travis-ci.org/sztomi/callme.svg?branch=master)](https://travis-ci.org/sztomi/callme) [![Build status](https://ci.appveyor.com/api/projects/status/9lft2tlamcox8epq?svg=true)](https://ci.appveyor.com/project/sztomi/callme) [![Coverage Status](https://coveralls.io/repos/sztomi/callme/badge.svg?branch=master&service=github)](https://coveralls.io/github/sztomi/callme?branch=master)

`callme` may be the easiest RPC library for C++! At least, that's what I hope to achieve. It is built using modern C++14, and as such, requires a very recent compiler.

The library uses a recent (continually updated) version of the msgpack headers. I decided
to not require it as a dependency but rather keep it in the repo, because it is quite small
and header-only.

# Work in progress

This library is a work in progress. Some things might not work, periodically break or change.

# Example

## Server

Creating a server is quite simple in callme.

```cpp
#include "callme/server.h"
#include <iostream>

void foo() {
    std::cout << "foo was called!" << std::endl;
}

int main(int argc, char *argv[]) {
    // Create a server that listens on port 8080
    callme::server srv("0.0.0.0", 8080);

    // Binding the name "foo" to free function foo.
    // note: the signature is automatically captured
    srv.bind("foo", &foo);

    // Binding a lambda function to the name "add".
    srv.bind("add", [](int a, int b) {
        return a + b;
    });

    // Run the server loop.
    srv.run();

    return 0;
}
```

When `srv.run()` is called, `callme` starts the server loop which listens to incoming connections
and tries to dispatch calls to the bound functions. The functions are called from the thread where
`run` was called from.

## Client

TBD


# Why another msgpack-RPC implementation?

The [original implementation for C++](https://github.com/msgpack-rpc/msgpack-rpc-cpp) was
discontinued. It lives on as [jubatus-msgpack-rpc](https://github.com/jubatus/jubatus-msgpack-rpc/tree/master/cpp), but this implemenation has a couple of undesirable requirements (which make perfect sense in the context of the library as part of Jubatus).

  * Autoconf-based build. Not only autoconf is dated, it also pretty much requires a Unix platfrom.
    Yes, cygwin is a workable alternative, but it is not completely native on windows, and means
    (almost) no MSVC support.
  * Lesser-known dependencies (it uses MPIO for the async event loop, which is yet another
    autoconf-based build, also part of Jubatus).
  * Awkward server implementation. The server implementor is required to unpack the parameters
    manually and dispatch the call based on the name of the methods.
  * At this moment (November 2015) it [doesn't compile](https://github.com/jubatus/jubatus-msgpack-rpc/issues/21) with the latest msgpack library.
  * Somewhat lacking documentation.

`callme` tackles/plans to tackle these issues the following way:

  * CMake build. Even though [not the most pleasant build system](http://szelei.me/cmake-is-not-great/) to work with, it provides a reliably reproducible build for the end-user. `callme` has a priority on ease of build for the target platforms/compilers.
  * Modern interface which provides minimalistic binding for many kinds of functors, allowing the
    library to dispatch functions automatically.
  * Extensive documentation.

On top of that, it also provides a code generator utility which can generate strongly typed,
"batteries included" clients and servers from a class header file.

# Thanks

`callme` builds on the shoulders of great projects. These are:

  * [MessagePack implementation for C and C++](https://github.com/msgpack/msgpack-c) ([website](http://msgpack.org/))
  * [libuv](https://github.com/libuv/libuv) ([website](http://libuv.org/))
  * [wheels](https://github.com/rmartinho/wheels) by Martinho Fernandes

