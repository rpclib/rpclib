#include "callme/server.h"

#include "easyloggingpp.h"

INITIALIZE_EASYLOGGINGPP

void foo() {
    auto log = el::Loggers::getLogger("callme");
    log->info("foo was called!");
}

int add(int a, int b) {
    auto log = el::Loggers::getLogger("callme");
    log->info("add(%v, %v) was called!", a, b);
    return a + b;
}

int main(int argc, char* argv[]) {
    auto log = el::Loggers::getLogger("callme");
    log->info("Creating server");
    callme::server srv("0.0.0.0", 8080);
    srv.disp_.bind("foo", &foo);
    srv.disp_.bind("add", &add);
    srv.run();
    return 0;
}
