# Primer

Welcome to the Primer! This document is a tutorial introduction to `callme` for absolute beginners. If you are new to the library and prefer detailed instructions, this is the right place. If short examples with less explanation work better for you, check out the [Cookbook](cookbook.md)!

The tutorial is sturctured as follows: in the first part, writing servers is explained with one simple and one more advanced example. In the second part, the corresponding clients are implemented.

## Prerequisites

Knowledge-wise, this tutorial assumes that you have an intermediate grasp of C++ and that you have an idea of what [RPC](https://en.wikipedia.org/wiki/Remote_procedure_call) (Remote Procedure Call) is.

For your build environment, make sure that you are able to compile and link a program with
`callme`. The [Getting Started](gettingstarted.md) page can help you with that.

## Introduction

`callme` is a RPC library that provides both a _client_ and a _server_ implementation. The _server_ allows you to expose functions of your program to be called remotely, while the _client_ allows you to call functions of servers. You can use the `callme` client and server in tandem (even in the same program, if you want to), but it's not a requirement. 

As other RPC libraries, `callme` is a good candidate for inter-process communication. Also, there exist many implementations of the protocol in a large amount of languages, which makes it a possible inter-language communication bridge.

msgpack-RPC is the protocol that `callme` uses for dispatching and encoding calls to functions. The protocol is based on [msgpack](http://msgpack.org), a fast and compact format. For details on how exactly it is structured, see the [Specification](spec.md) chapter.

# Writing servers

In the first part of this tutorial, we will learn about writing server applications.

IMPORTANT: msgpack-RPC defines *zero* security features. If you need secure communication, you have to implement it above or below the protocol!

## Calculator, the "Hello World" of RPC libraries.

Our first server application will expose four functions: `add`, `subtract`, `multiply`, `divide`. The skeleton of this application will look like this:

```cpp
#include "callme/server.h"

// The functions to expose
double add(double a, double b)      { return a + b; }
double subtract(double a, double b) { return a - b; }
double multiply(double a, double b) { return a * b; }
double divide(double a, double b)   { return a / b; }

int main() {
    // server code goes here
    return 0;
}
```

So how do we define a server? First, we create the server object:

```cpp
    callme::server srv(8080);
```

This server will listen on port 8080 (but not right away after construction - we need to `run` it). Next, we _bind_ the functions to names:

```cpp
    srv.bind("add", &add);
    srv.bind("subtract", &subtract);
    srv.bind("multiply", &multiply);
    srv.bind("divide", &divide);
```

These are the names that the client can use to call our functions. There is nothing stopping you from binding functions to names that are different from what they are named in your source, too. But usually you will want to avoid that and aim to keep the principle of least surprises. It is also possible to bind the same function to multiple names.

NOTE: Under the hood, each `bind` statement generates at compile-time a wrapper function that takes a `msgpack` object, decodes it into the real parameters of the bound function (if any) and calls it. If the function has a return value the wrapper is generated so that it encodes the result as a `msgpack` object which the server can send to the client in a response. More information on this mechanism can be found in the [Design](design.md) chapter.

After we exposed the function, we need to `run` the server:

```cpp
    srv.run();
```

`run` is a blocking function, but it also has a non-blocking pair called `async_run`. When `run` is called, the server starts listening on the port we gave it above and its internal event loop will be continously processed.

This is now a functioning (although, in some ways, incomplete) server. The complete listing so far:

```cpp
#include "callme/server.h"

// The functions to expose
double add(double a, double b)      { return a + b; }
double subtract(double a, double b) { return a - b; }
double multiply(double a, double b) { return a * b; }
double divide(double a, double b)   { return a / b; }

int main() {
    callme::server srv(8080);
    srv.bind("add", &add);
    srv.bind("subtract", &subtract);
    srv.bind("multiply", &multiply);
    srv.bind("divide", &divide);
    srv.run();

    return 0;
}
```

TIP: Free functions are not the only thing you can `bind`. It also accepts _lambda expressions_ (both capturing and non-capturing), `std::function` objects and _custom functors_. To bind class members, you can wrap an instance with the member function pointer in a lambda or using `std::bind`.

## Responding with errors

There is, however, a huge issue with this server. Did you spot it? A client can easily make it crash just by calling `divide` with a 0 divider (causing division by zero). What can we do about this? Well of course, we can just check the divider and _not_ perform the division if it happens to be zero. This is enough to avoid the crash, but not enough for a smooth workflow: the client would get an arbitrary result which is most likely not what they want.

Luckily, the msgpack-rpc protocol supports error signaling. We need to modify our `divide` function a little bit to utilize this functionality:


```cpp
#include "callme/server.h"
#include "callme/this_handler.h"

double divide(double a, double b) {
    if (b == 0) {
        callme::this_handler().set_error("Division by zero");
    }
    return a / b;
}
```

You might be puzzled about why we are not returning after setting the error. The reason for this is that `set_error` throws an internal exception that is handled inside the library. This might be an implementation detail, but it's good to know what happens here (and it's unlikely to change).

Now, with the added error handling, our server is bullet-proof. Or is it?

## What about _my_ exceptions?

Our little calculator server is pretty stable at this point, but real-world applications often have to deal with exceptions. In general, exceptions should be handled at the library users' discretion (that is, on the handler level), so by default, `callme` doesn't do anything with them. If an exception leaves the handler, that is an unhandled exception and your server will crash. Yet, there are cases when you can't or don't want to handle exceptions in the handler. To facilitate this, `callme` provides a way to automatically turn exceptions into RPC errors:

```cpp
    srv.suppress_exceptions(true);
```

With this, you can call functions that throw or throw exceptions of your own in the bound function.

```cpp
double divide(double a, double b) {
    if (b == 0) {
        callme::this_handler().set_error("Division by zero");
    }
    else if (b == 1) {
        throw std::runtime_error("Come on!");
    }
    throw std::logic_error("What am I doing here?");
}
```

So yes, this means that if you set `suppress_excpetions` to `true`, you might as well signal errors from handlers by throwing exceptions. Be advised, that `set_error` is still valid and remains the preferred way to do so.

What exactly happens to the exception? `callme` will try to catch `std::exceptions` and use their `what()` members to get a string representation which it sets as an error. 

What if you throw something that is not a `std::exception`-descendant? First of all, shame on you. Second, `callme` will send an error message letting your clients know how that you threw something that is not a `std::exception` (*shaming you in front of your clients*).

## A more complicated server - Parallel mandelbrot-generation

The following example demonstrates parallel processing and binding custom data types. The server itself will have two functions: one for getting the current date and time, and one for getting a rendering of the mandelbrot set. The two functions can be called asynchronously by a client.

### Using custom types as parameters

Anything that msgpack can process can be used as a parameter or return value for a bound function. In order to teach msgpack about your custom types, you need to use the `MSGPACK_DEFINE_ARRAY` or `MSGPACK_DEFINE_MAP` macros.

TIP: The difference is that the array only contains the data values after each other, while the map also contains the names of the values. The latter gives more flexibility, the former is more compact.

In our mandelbrot example, we will want to send pixel data to the clients, so let's define a struct:

```cpp
struct pixel {
    unsigned char r, g, b;
    MSGPACK_DEFINE_ARRAY(r, g, b)
};

using pixel_data = std::vector<pixel>;
```

We will share this definition between the client and server, so for our purposes it's best to put it in a common header.

Like in the first example, we create the server and bind the functions we expose. This time we are using lambdas as the bound functions.

```cpp
    callme::server srv(8080);

    srv.bind("get_time", []() {
        time_t rawtime;
        struct tm *timeinfo;
        time (&rawtime);
        timeinfo = localtime(&rawtime);
        return asctime(timeinfo);
    });

    srv.bind("get_mandelbrot", [&](int width, int height) {
        pixel_data data;
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                // ...
            }
        }

        return data;
    });
```

The exact contents of these functions is not a concern for our purposes, just note that the `get_time` returns a value very quickly, while `get_mandelbrot` computes a large array of numbers for several seconds.

## Running the server asynchrously and utilizing workers

In the first example, we called the blocking `run` function of the server to start it. Here, we are going to use `async_run`. There are two important differences.

. `run` blocks, `async_run` returns after starting the server.
. `async_run` supports spawning worker threads for executing the bound functions.

In this example, we call it like this:

```cpp
    srv.async_run(2);
```

This will spawn two worker threads in the server (so now there are three in the program, because the main thread already exists). The threads will wait until there is something to do.

NOTE: "Something" is not only executing handlers. Processing network I/O is also part of the work that threads can take. You don't need an extra thread per connection though, because processing the I/O is typically not very processor-intensive.

Now this server can take a call to `get_mandelbrot`, start executing it and in the meantime it can finish multiple `get_time` calls. The handlers are only executed by these worker threads, the main thread is free to continue.

# Writing clients

Creating msgpack-rpc clients with `callme` happens in a very similarly to servers. Mirroring the server examples above, we will implement their corresponding clients.

## The Calculator client

The `client` object is instantiated like this:

```cpp
    callme::client client("127.0.0.1", 8080);
```

The important difference, compared to a server, is that we also need to specify the host to connect to.

Another difference is that the client tries to connect to the server right away during construction (but the construction of the client is not a blocking call). The client object can be used right away:

```cpp
    auto result = client.call("add", 2, 3).as<int>();
```

### The anatomy of a `call`

`call` does a couple of things:

    * If the client is not yet connected to the server, it waits until it connects (this might block until the connection is established)
    * Sends a "call" message to the server
    * Waits for the response and returns it as a msgpack object - this blocks until the response is read.

In the example above, you can see how getting a strongly typed value from the result is done: using the `as` member template. This takes the msgpack object and tries to deserialize it into the type given. If that fails, you will get a `type_error`.

`call` takes at least one parameter (the name of the function to call), and an arbitrary number and type of other paramters that are meant to be passed to the function being called. Each parameter has to be serializable by msgpack.

TIP: See [msgpack adaptors](https://github.com/msgpack/msgpack-c/wiki/v1_1_cpp_adaptor) for more information on serializing and deserializing custom types.

## Error handling

Any request that client makes, might potentially receive an error response. In msgpack-RPC, an error is an arbitrary object. `callme` allows you to handle these error objects by catching `callme::rpc_error` exceptions.

That's pretty much all we need for the calculator client.

## The Mandelbrot client

The client for the mandelbrot server above is interesting because we will take advantage of the multiple workers in the server. In order to do that, we need just one different step: instead of `call` we user `async_call`.

`async_call` is very similar to `call`, but it does not wait for the response. Instead, it will return a [future](http://en.cppreference.com/w/cpp/thread/future), allowing us to continue our program flow and retrieve the result later (which the server can compute in the meantime).

```cpp
    callme::client c("127.0.0.1", 8080);

    // this returns immediately:
    auto result_obj = c.async_call("get_mandelbrot", width, height);

    // we can now call another function and wait for its result:
    auto current_time = c.call("get_time").as<std::string>();

    // ... after some time, retrieve the result (optionally wait for it)
    auto result = result_obj.get().as<pixel_data>();
```

The call to `get_time` can be performed with `call` (no need for `async_call`), because the other call is running on a different worker.

IMPORTANT: **What would happen if our server only had one worker thread?** We would get the same output, but with more delay: The server would only start processing the `get_time` call after it finished executing and writing the response of `get_mandelbrot`. Essentially, a single-threaded server works in a "queue" fashion. The same thing would happen if the server was simple under heavy load.

### How do async servers relate to async clients and parallel execution in general?

Does the asynchonous nature of `async_call` depend on the server or the load of the server then? No, it does not. It's important to realize that `async_call` is still asynchronous even if the server does not execute requests in parallel. If there are multiple clients connected to the server, their requests are processed in a more queued manner (still two requests processed at the same time).

TIP: `callme` uses a simple convention: `foo` is a synchronous call, `async_foo` is asynchronous. This conventions was adapted from Asio. The latter only means that the call returns "immediately" (or rather, very quickly and without finishing all of the work).

The two worker threads in the mandelbrot server can serve two clients in parallel. Or two calls of the same client, which happens in the example. In order to be able to send two requests in an interleaved fashion, we first use `async_call` which allows the control flow of the client to continue.
