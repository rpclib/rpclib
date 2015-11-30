#include "callme/server.h"

#include "easyloggingpp.h"

#ifndef CALLME_LOGGING
INITIALIZE_EASYLOGGINGPP
#endif

void foo() {
    auto log = el::Loggers::getLogger("callme");
    log->info("foo was called!");
}

void bad(int x) {
    if (x == 5) {
        throw std::runtime_error("x == 5. I really don't like 5.");
    }
}

int main(int argc, char *argv[]) {
    auto log = el::Loggers::getLogger("callme");
    log->info("Creating server");
    callme::server srv("0.0.0.0", 8080);

    srv.bind("foo", &foo);

    srv.bind("add", [](int a, int b) {
        auto log = el::Loggers::getLogger("callme");
        log->info("add(%v, %v) was called!", a, b);
        return a + b;
    });

    srv.bind("bad", &bad);

    srv.run();
    return 0;
}
