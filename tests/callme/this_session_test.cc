#include <chrono>
#include <thread>

#include "gtest/gtest.h"

#include "callme/client.h"
#include "callme/server.h"
#include "callme/this_session.h"

#include "testutils.h"

using namespace callme::testutils;
using namespace callme;

const uint16_t test_port = 8080;

class this_session_test : public testing::Test {
public:
    this_session_test() : s(test_port), c("127.0.0.1", test_port) {}

protected:
    callme::server s;
    callme::client c;
};

TEST_F(this_session_test, post_exit) {
    using namespace std::chrono_literals;
    s.bind("exit", []() { callme::this_session().post_exit(); });
    s.async_run();
    c.call("exit");
    std::this_thread::sleep_for(100ms);
    auto f = c.async_call("exit");
    EXPECT_EQ(f.wait_for(50ms), std::future_status::timeout);
    EXPECT_EQ(c.get_connection_state(), client::connection_state::disconnected);
}

TEST_F(this_session_test, post_exit_specific_to_session) {
    using namespace std::chrono_literals;
    callme::client c2("127.0.0.1", test_port);
    s.bind("exit", [](bool do_exit) {
        if (do_exit) {
            callme::this_session().post_exit();
        }
    });
    s.async_run();

    c2.call("exit", false);
    c.call("exit", true);
    std::this_thread::sleep_for(100ms);
    auto f = c.async_call("exit");
    c2.call("exit", false);
    EXPECT_EQ(f.wait_for(50ms), std::future_status::timeout);
    EXPECT_EQ(c.get_connection_state(), client::connection_state::disconnected);
    EXPECT_EQ(c2.get_connection_state(), client::connection_state::connected);
}
