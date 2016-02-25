#include <chrono>
#include <thread>

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "callme/client.h"
#include "callme/server.h"
#include "testutils.h"

using namespace callme::testutils;
using namespace std::literals::chrono_literals;

const int test_port = 8080;

class server_workers_test : public testing::Test {
public:
    server_workers_test()
        : s("localhost", test_port), long_count(0), short_count(0) {
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
    callme::server s;
    std::atomic_int long_count, short_count;
};

TEST_F(server_workers_test, single_worker) {
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

TEST_F(server_workers_test, multiple_workers) {
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

class server_error_handling : public testing::Test {
public:
    server_error_handling() : s("localhost", test_port) {
        s.bind("blue", []() {
            throw std::runtime_error("I'm blue daba dee daba die");
        });
        s.bind("red", []() { throw "Am I evil? Yes I am."; });
        s.async_run();
    }

protected:
    callme::server s;
};

#ifndef WIN32
TEST_F(server_error_handling, no_suppress) {
    callme::client c("127.0.0.1", test_port);
    s.suppress_exceptions(false);
    EXPECT_DEATH({ c.call("blue"); }, "");
    EXPECT_DEATH({ c.call("red"); }, "");
}
#endif

TEST_F(server_error_handling, suppress) {
    s.suppress_exceptions(true);
    callme::client c("127.0.0.1", test_port);
    // this seems like the opposite check, but the client throwing
    // the exception means that it reached the other side, i.e.
    // the server suppressed it.
    EXPECT_THROW(c.call("blue"), std::runtime_error);
    EXPECT_THROW(c.call("red"), std::runtime_error);
    EXPECT_THROW(c.call("green"), std::runtime_error);
    EXPECT_THROW(c.call("blue", 1), std::runtime_error);
}

TEST_F(server_error_handling, suppress_right_msg) {
    s.suppress_exceptions(true);
    callme::client c("127.0.0.1", test_port);

    try {
        c.call("blue");
        FAIL() << "There was no exception thrown.";
    } catch (std::exception &e) {
        EXPECT_TRUE(str_match(e.what(), ".*I'm blue daba dee daba die.*"));
    }

    try {
        c.call("red");
        FAIL() << "There was no exception thrown.";
    } catch (std::exception &e) {
        EXPECT_FALSE(str_match(e.what(), ".*Am I evil.*"));
        EXPECT_TRUE(str_match(e.what(), ".*not derived from std::exception.*"));
    }
}

TEST_F(server_error_handling, no_such_method_right_msg) {
    s.suppress_exceptions(true);
    callme::client c("127.0.0.1", test_port);
    try {
        c.call("green");
        FAIL() << "There was no exception thrown.";
    } catch (std::exception &e) {
        EXPECT_TRUE(str_match(e.what(), ".*could not find.*"));
    }
}

TEST_F(server_error_handling, wrong_arg_count_void_zeroarg) {
    s.suppress_exceptions(true);
    callme::client c("127.0.0.1", test_port);
    try {
        c.call("blue", 1);
        FAIL() << "There was no exception thrown.";
    } catch (std::exception &e) {
        EXPECT_TRUE(str_match(e.what(), ".*invalid number of arguments.*"));
    }
}

class dispatch_unicode : public testing::Test {
public:
    dispatch_unicode()
        : s("localhost", 8080),
          str_utf8("árvíztűrő tükörfúrógép") {
        s.bind("utf", [](std::string const &p) { return p; });
        s.async_run();
    }

protected:
    callme::server s;
    std::string str_utf8;
};

TEST_F(dispatch_unicode, narrow_unicode) {
    callme::client c("127.0.0.1", test_port);
    EXPECT_EQ(str_utf8, c.call("utf", str_utf8).as<std::string>());
}
