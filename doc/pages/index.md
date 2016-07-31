# rpclib - modern msgpack-rpc for C++

Welcome to the documentation of `rpclib`! `rpclib` is a msgpack-rpc implementation written using modern C++. The goal of this library is to provide a simple, no-nonsense RPC solution.

## Documentation

  * [Getting started](gettingstarted.md): Requirements, installation - everything you need to be able to start using `rpclib`
  * [Primer](primer.md): A longer tutorial introduction to using the library, intended for newcomers
  * [Cookbook](cookbook.md): Recipes for common tasks the library was designed for handling, in the form of short examples
  * [Building](building.md): Basic and advanced building options.
  * [Glossary](glossary.md): Collection of definitions used by the library.
  * [Reference](reference.md): Complete reference of the `rpclib` API.
  * [MsgPack-RPC specification](spec.md): The specification of the Msgpack-RPC protocol, as `rpclib` implements it
  * [Roadmap](roadmap.md): Planned features

## What does it look like?

Here is a very simple example, a kind of hello world for network-related libraries:

```cpp
#include "rpc/server.h"

int main() {
    rpc::server srv(8080);

    srv.bind("echo", [](std::string const& s) {
        return s;
    });

    srv.run();
    return 0;
}
```

```cpp
#include "rpc/client.h"

#include <iostream>

int main() {
    rpc::client c("localhost", 8080);

    std::string text;
    while (std::getline(std::cin, text)) {
        if (!text.empty()) {
            std::string result(c.call("echo", text).as<std::string>());
            std::cout << "> " <<  result << std::endl;
        }
    }
}
```
