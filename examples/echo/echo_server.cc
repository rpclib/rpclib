#include "rpc/server.h"

int main() {
    rpc::server srv(8080);

    srv.bind("echo", [](std::string const& s) {
        return s;
    });

    srv.run();
    return 0;
}
