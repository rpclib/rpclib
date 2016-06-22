# Cookbook

Welcome to the Cookbook! This is a collection of short examples that allows you to quickly learn
the ins and outs of `rpclib`. This guide is written in the spirit of "less talk and more code". If
you prefer detailed instructions and explanation, you might want to start with the
[Primer](primer.md).

## Server examples

### Creating a server

```cpp
#include "rpc/server.h"

int main() {
    rpc::server srv(8080); // listen on TCP port 8080

    srv.run(); // blocking call
    return 0;
}
```

### Binding (exposing) free functions 

```cpp
#include "rpc/server.h"
#include <iostream>

void foo() {
    std::cout << "Hey, I'm a free function." << std::endl;
}

int main() {
    rpc::server srv(8080); // listen on TCP port 8080

    srv.bind("foo", &foo);

    srv.run(); // blocking call, handlers run on this thread.
    return 0;
}
```

### Binding lambdas

```cpp
#include "rpc/server.h"

int main() {
    rpc::server srv(8080); // listen on TCP port 8080

    srv.bind("hl3", []() { 
        std::cout << "Hey, I'm a lambda!" << std::endl; 
    });

    srv.run(); // blocking call
    return 0;
}
```

### Binding member functions

Consider this class:

```cpp
class foo_class {
public:
    void quaz() {
        std::cout << "Hey, I'm a member function!" << std::endl;
    }
};
```

#### Version 1: Binding through a lambda (preferred)

```cpp
#include "rpc/server.h"

int main() {
    rpc::server srv(8080); // listen on TCP port 8080
    foo_class foo_obj;

    srv.bind("quaz", [&foo_obj](){ foo_obj.quaz(); }); 

    srv.run(); // blocking call

    // NOTE: you have to make sure that the lifetime of foo_obj 
    // exceeds that of the server.

    return 0;
}
```

#### Version 2: using `std::bind`

```cpp
#include <functional>

#include "rpc/server.h"

int main() {
    rpc::server srv(8080); // listen on TCP port 8080
    foo_class foo_obj;

    std::function<void()> functor{std::bind(&foo_class::quaz, &foo_obj)};

    srv.bind("quaz", functor);

    srv.run(); // blocking call

    // NOTE: you have to make sure that the lifetime of foo_obj 
    // exceeds that of the server.

    return 0;
}
```

### Multiple worker threads

```cpp
#include "rpc/server.h"
#include <iostream>

void foo() {
    std::cout << "Hey, I'm a free function." << std::endl;
}

int main() {
    rpc::server srv(8080); // listen on TCP port 8080

    srv.bind("foo", &foo);

    constexpr size_t thread_count = 8;

    srv.async_run(thread_count); // non-blocking call, handlers execute on one of the workers

    std::cin.ignore();
    return 0;
}
```

### Responding with errors

```cpp
#include "rpc/server.h"
#include "rpc/this_handler.h"

int main() {
    rpc::server srv(8080); // listen on TCP port 8080

    srv.bind("error", []() { 
        auto err_obj = std::make_tuple(13, "Errors are arbitrary objects");
        rpc::this_handler().respond_error(err_obj);
    });

    srv.run(); // blocking call
    return 0;
}
```

### Responding with arbitrary objects

Even though C++ isn't, `msgpack-rpc` is very lenient on types. Your client might be implemented in
a dynamic language.

```cpp
#include "rpc/server.h"
#include "rpc/this_handler.h"

int main() {
    rpc::server srv(8080); // listen on TCP port 8080

    srv.bind("oh", [](bool quirky) -> std::string { 
        if (quirky) {
            rpc::this_handler().respond(5);
        }
        return "I'm not quirky.";
    });

    srv.run(); // blocking call
    return 0;
}
```

### Disabling response

This prevents the server from ever writing a response to a particular call.

```cpp
#include "rpc/server.h"
#include "rpc/this_handler.h"

int main() {
    rpc::server srv(8080); // listen on TCP port 8080

    srv.bind("plz_respond", [](bool nice) {
        if (nice) {
            rpc::this_handler().disable_response();
        }
        return "ok";
    });

    srv.run(); // blocking call
    return 0;
}
```

### Exiting a session

A session represents a client connection on the server. All ongoing writes and reads are completed first.

```cpp
#include "rpc/server.h"
#include "rpc/this_session.h"

int main() {
    rpc::server srv(8080); // listen on TCP port 8080

    srv.bind("exit", []() {
        rpc::this_session().post_exit(); // post exit to the queue
    });

    srv.run(); // blocking call
    return 0;
}
```

`post_exit` will add exiting the session as a work item to the queue. This means that exiting is
not instantenous. The TCP connection will be closed gracefully.

### Stopping a server

To gracefully stop all sessions on the server. All ongoing writes and reads are completed first.

```cpp
#include "rpc/server.h"
#include "rpc/this_server.h"

int main() {
    rpc::server srv(8080); // listen on TCP port 8080

    srv.bind("stop_server", []() {
        rpc::this_server().stop();
    });

    srv.run(); // blocking call
    return 0;
}
```

## Client examples

### Creating a client

```cpp
#include "rpc/client.h"

int main() {
    rpc::client c("127.0.0.1", 8080);

    // client initiates async connection upon creation
    return 0;

    // destructor of client disconnects
}
```

### Calling functions

```cpp
#include "rpc/client.h"

int main() {
    rpc::client c("127.0.0.1", 8080);

    // client initiates async connection upon creation

    // call blocks until:
    // - connection is established
    // - result is read
    c.call("foo", 2, 3.3, "str"); 

    return 0;
}
```

### Getting return values

```cpp
#include "rpc/client.h"

int main() {
    rpc::client c("127.0.0.1", 8080);
    int a = c.call("add", 2, 3).as<int>(); 

    return 0;
}
```

### Calling functions asynchronously

```cpp
#include "rpc/client.h"

int main() {
    rpc::client c("127.0.0.1", 8080);

    auto a_future = c.call_async("add", 2, 3); // non-blocking, returns std::future

    int a = a_future.get().as<int>(); // possibly blocks if the result is not yet available

    return 0;
}
```

### Querying the connection state

```cpp
#include "rpc/client.h"

int main() {
    rpc::client c("127.0.0.1", 8080);
    client::connection_state cs = c.get_connection_state();

    return 0;
}
```
