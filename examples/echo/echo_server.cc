#include "rpc/server.h"

int main() {
    rpc::server srv(rpc::constants::DEFAULT_PORT);

    srv.bind("echo", [](std::string const& s) {
        return s;
    });

    srv.run();
    return 0;
}
