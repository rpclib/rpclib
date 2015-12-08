#include <iostream>
#include "callme/server.h"

void foo() { std::cout << "foo was called!" << std::endl; }

void bad(int x) {
    if (x == 5) {
        throw std::runtime_error("x == 5. I really don't like 5.");
    }
}
int main(int argc, char *argv[]) {
    // Create a server that listens on port 8080
    callme::server srv("0.0.0.0", 8080);

    // Binding the name "foo" to free function foo.
    // note: the signature is automatically captured
    srv.bind("foo", &foo);

    // Binding a lambda function to the name "add".
    srv.bind("add", [](int a, int b) { return a + b; });

    srv.suppress_exceptions(true);
    // Throwing an exception will cause the server to write
    // an error response.
    srv.bind("bad", &bad);

    // Run the server loop.
    srv.run();

    return 0;
}
