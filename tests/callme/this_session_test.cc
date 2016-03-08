#include <chrono>
#include <thread> 

#include "gtest/gtest.h"

#include "callme/client.h"
#include "callme/server.h"

#include "testutils.h"

using namespace callme::testutils;

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
}
