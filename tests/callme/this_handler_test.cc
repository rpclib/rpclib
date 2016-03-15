#include <chrono>

#include "gtest/gtest.h"

#include "callme/client.h"
#include "callme/server.h"
#include "callme/this_handler.h"

#include "testutils.h"

using namespace callme::testutils;

const uint16_t test_port = 8080;

class this_handler_test : public testing::Test {
public:
    this_handler_test() : s(test_port) {}

protected:
    callme::server s;
};

TEST_F(this_handler_test, set_error) {
    s.suppress_exceptions(false);
    s.bind("errfunc", []() {
        callme::this_handler().set_error("Imma let you finish, but.");
    });
    s.async_run();

    callme::client c("127.0.0.1", test_port);
    EXPECT_THROW(c.call("errfunc"), std::runtime_error);

    try {
        c.call("errfunc");
        FAIL() << "There was no exception thrown.";
    } catch (std::runtime_error &e) {
        EXPECT_TRUE(str_match(e.what(), ".*Imma let you finish, but.*"));
    }
}

TEST_F(this_handler_test, clear_error) {
    s.suppress_exceptions(false);
    s.bind("errfunc", []() {
        callme::this_handler().set_error("Imma let you finish, but.");
        callme::this_handler().clear_error();
    });
    s.async_run();

    callme::client c("127.0.0.1", test_port);
    EXPECT_NO_THROW(c.call("errfunc"));
}

TEST_F(this_handler_test, set_special_response) {
    std::string text("What? You thought I was a number?");
    s.bind("spec_func", [text](bool special) {
        if (special) {
            callme::this_handler().set_special_response(text);
        }
        return 5;
    });
    s.async_run();

    callme::client c("127.0.0.1", test_port);
    EXPECT_EQ(c.call("spec_func", false).as<int>(), 5);
    EXPECT_EQ(c.call("spec_func", true).as<std::string>(), text);
    EXPECT_THROW(c.call("spec_func", true).as<int>(), msgpack::type_error);
}

TEST_F(this_handler_test, clear_special_response) {
    std::string text("What? You thought I was a number?");
    s.bind("spec_func", [text](bool special) {
        if (special) {
            callme::this_handler().set_special_response(text);
            callme::this_handler().clear_special_response();
        }
        return 5;
    });
    s.async_run();

    callme::client c("127.0.0.1", test_port);
    EXPECT_EQ(c.call("spec_func", false).as<int>(), 5);
    EXPECT_EQ(c.call("spec_func", true).as<int>(), 5);
    EXPECT_THROW(c.call("spec_func", true).as<std::string>(),
                 msgpack::type_error);
}

TEST_F(this_handler_test, disable_response) {
    using namespace std::chrono_literals;
    s.bind("noresp", []() { callme::this_handler().disable_response(); });
    s.async_run();

    callme::client c("127.0.0.1", test_port);
    auto f = c.async_call("noresp");
    EXPECT_EQ(f.wait_for(50ms), std::future_status::timeout);
}

TEST_F(this_handler_test, enable_response) {
    using namespace std::chrono_literals;
    s.bind("noresp", []() { callme::this_handler().disable_response(); });
    s.async_run();

    callme::client c("127.0.0.1", test_port);
    auto f = c.async_call("noresp");
    EXPECT_EQ(f.wait_for(50ms), std::future_status::timeout);
}
