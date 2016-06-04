
# callme ![MIT](https://img.shields.io/badge/license-MIT-blue.svg) [![Build Status](https://travis-ci.org/sztomi/callme.svg?branch=master)](https://travis-ci.org/sztomi/callme) [![Build status](https://ci.appveyor.com/api/projects/status/9lft2tlamcox8epq?svg=true)](https://ci.appveyor.com/project/sztomi/callme) [![Coverage Status](https://coveralls.io/repos/sztomi/callme/badge.svg?branch=master&service=github)](https://coveralls.io/github/sztomi/callme?branch=master) ![Coverity](https://scan.coverity.com/projects/7259/badge.svg?flat=1)

`rpclib` may be the easiest RPC library for C++! It is built using modern C++14, and as such, requires a very recent compiler.

The library uses a recent (continually updated) version of the msgpack headers. I decided
to not require it as a dependency but rather keep it in the repo, because it is quite small
and header-only.

# Work in progress

This library is a work in progress. Some things might not work, periodically break or change.

# Building

To build `rpclib` you can do the following:

```
git clone git@github.com:sztomi/rpc.git
cd rpc
mkdir build && cd build
cmake ..
make
```

There are of course many other ways to build the library, depending on your target compiler,
platform and individual needs. Alternatively, you can also take the source files from the lib
directory and the include files from include and integrate them in your project.

TBD: cmake options, preprocessor definitions, handling builtin dependencies.

# What does it look like?

## Server

Creating a server is quite simple in rpc.

```cpp
#include "rpc/server.h"
#include <iostream>

void foo() {
    std::cout << "foo was called!" << std::endl;
}

int main(int argc, char *argv[]) {
    // Create a server that listens on port 8080
    rpc::server srv("0.0.0.0", 8080);

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
`run` was called from. There is a convenience function called `async_run` that starts a new thread.

## Client

```cpp
#include <iostream>
#include "rpc/client.h"

int main() {
    rpc::client client("127.0.0.1", 8080);
    auto result = client.call("add", 2, 3).as<int>();
    std::cout << "The result is: " << result << std::endl;
    return 0;
}
```

# Thanks

`rpclib` builds on the shoulders of giants. These great projects are, in no particular order:

  * [MessagePack implementation for C and C++](https://github.com/msgpack/msgpack-c) by Takatoshi Kondo ([website](http://msgpack.org/))
  * [asio](https://github.com/chriskohlhoff/asio) by Christopher Kohlhoff ([website](http://think-async.com/Asio))
  * [cppformat](https://github.com/cppformat/cppformat) by Victor Zverovich ([website](http://cppformat.github.io/latest/index.html))
  * [googletest](https://github.com/google/googletest) by Google
  * [wheels](https://github.com/rmartinho/wheels) by Martinho Fernandes

Shoutouts to

  * [Appveyor](http://www.appveyor.com)
  * [Travis CI](https://travis-ci.org)
  * [Coveralls.io](http://coveralls.io)
  * [Coverity](http://www.coverity.com)
  * [ASan & TSan](https://github.com/google/sanitizers) helped resolving many bugs.




