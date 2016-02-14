#include <chrono>
#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "callme/server.h"
#include "callme/client.h"
#include "testutils.h"

using namespace callme::testutils;
using namespace std::literals::chrono_literals;

class server_test : public testing::Test {
public:
    server_test() : s("localhost", test_port), long_count(0), short_count(0) {
        s.bind("long_func", [this]() {
            std::this_thread::sleep_for(500ms);
            ++long_count;
        });
        s.bind("short_func", [this]() {
            std::this_thread::sleep_for(100ms);
            ++short_count;
        });
    }

protected:
    static const int test_port = 8080;
    callme::server s;
    std::atomic_int long_count, short_count;
};


TEST_F(server_test, single_worker) {
    const std::size_t workers = 1;
    s.async_run(workers);
    callme::client c("127.0.0.1", test_port);
    auto ft_long = c.async_call("long_func");
    auto ft_short = c.async_call("short_func");
    ft_short.wait();

    EXPECT_EQ(1, short_count);
    EXPECT_EQ(1, long_count);

    ft_long.wait();
    EXPECT_EQ(1, short_count);
    EXPECT_EQ(1, long_count);
}

TEST_F(server_test, multiple_workers) {
    const std::size_t workers = 2;
    s.async_run(workers);
    callme::client c("127.0.0.1", test_port);
    auto ft_long = c.async_call("long_func");
    auto ft_short = c.async_call("short_func");
    ft_short.wait();

    EXPECT_EQ(1, short_count);
    EXPECT_EQ(0, long_count);

    ft_long.wait();
    EXPECT_EQ(1, short_count);
    EXPECT_EQ(1, long_count);
}
