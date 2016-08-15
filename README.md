
# rpclib ![MIT](https://img.shields.io/badge/license-MIT-blue.svg) [![Build Status](https://travis-ci.org/sztomi/rpclib.svg?branch=master)](https://travis-ci.org/sztomi/rpclib) [![Build status](https://ci.appveyor.com/api/projects/status/9lft2tlamcox8epq?svg=true)](https://ci.appveyor.com/project/sztomi/rpclib) [![Coverage Status](https://coveralls.io/repos/sztomi/rpclib/badge.svg?branch=master&service=github)](https://coveralls.io/github/sztomi/rpclib?branch=master) ![Coverity](https://scan.coverity.com/projects/7259/badge.svg?flat=1)

`rpclib` is a RPC library for C++, providing both a client and server implementation. It is built using modern C++14, and as such, requires a recent compiler. Using the library, you can:

  * Expose functions of your program to be called via RPC with minimal boilerplate
  * Or call function through RPC with a syntax that is as natural as C++ permits (without code
    generation)

# What does it look like?

## Server

```cpp
#include <iostream>
#include "rpc/server.h"

void foo() {
    std::cout << "foo was called!" << std::endl;
}

int main(int argc, char *argv[]) {
    // Creating a server that listens on port 8080
    rpc::server srv(8080);

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

When `srv.run()` is called, `rpclib` starts the server loop which listens to incoming connections
and tries to dispatch calls to the bound functions. The functions are called from the thread where
`run` was called from. There is also `async_run` that spawns worker threads and returns
immediately.

## Client

```cpp
#include <iostream>
#include "rpc/client.h"

int main() {
    // Creating a client that connects to the localhost on port 8080
    rpc::client client("127.0.0.1", 8080);

    // Calling a function with paramters and converting the result to int
    auto result = client.call("add", 2, 3).as<int>();
    std::cout << "The result is: " << result << std::endl;
    return 0;
}
```

# Status

Most planned 1.0.0 features are done and tested; what you are seeing here is a preview of the
release (I'm hesitant to call it release candidate because there are known outstanding issues).

# Thanks

`rpclib` builds on the efforts of fantastic C++ projects. In no particular order:

  * [MessagePack implementation for C and C++](https://github.com/msgpack/msgpack-c) by Takatoshi Kondo ([website](http://msgpack.org/))
  * [asio](https://github.com/chriskohlhoff/asio) by Christopher Kohlhoff ([website](http://think-async.com/Asio))
  * [cppformat](https://github.com/cppformat/cppformat) (now renamed `fmtlib` by Victor Zverovich ([website](http://fmtlib.net))
  * [googletest](https://github.com/google/googletest) by Google
  * [wheels](https://github.com/rmartinho/wheels) by Martinho Fernandes

Shoutouts to

  * [Appveyor](http://www.appveyor.com)
  * [Travis CI](https://travis-ci.org)
  * [Coveralls.io](http://coveralls.io)
  * [Coverity](http://www.coverity.com)
  * [ASan & TSan](https://github.com/google/sanitizers) helped spotting and resolving many bugs.




