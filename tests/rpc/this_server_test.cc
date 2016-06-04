#include "gtest/gtest.h"

#include "rpc/client.h"
#include "rpc/server.h"
#include "rpc/this_server.h"

#include "testutils.h"

using namespace rpc::testutils;
using namespace rpc;

const uint16_t test_port = 8080;

class this_server_test : public testing::Test {
public:
    this_server_test()
        : s(test_port),
          c1("127.0.0.1", test_port),
          c2("127.0.0.1", test_port) {}

protected:
    rpc::server s;
    rpc::client c1;
    rpc::client c2;
};

TEST_F(this_server_test, stop) {
    using namespace std::chrono_literals;
    s.bind("stop_server", []() { rpc::this_server().stop(); });
    s.async_run();
    c1.call("stop_server");
    std::this_thread::sleep_for(100ms);

    EXPECT_EQ(c1.get_connection_state(),
              client::connection_state::disconnected);
    EXPECT_EQ(c2.get_connection_state(),
              client::connection_state::disconnected);
}
