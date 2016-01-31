#include "gtest/gtest.h"

#include "callme/client.h"
#include "callme/server.h"
#include "testutils.h"

using namespace callme::testutils;

class client_test : public testing::Test {
public:
    client_test() : s("localhost", test_port) {
        s.bind("dummy_void_zeroarg", [this]() { md.dummy_void_zeroarg(); });
        s.bind("dummy_void_singlearg",
               [this](int x) { md.dummy_void_singlearg(x); });
        s.bind("dummy_void_multiarg",
               [this](int x, int y) { md.dummy_void_multiarg(x, y); });
        s.async_run();
    }

protected:
    static const int test_port = 8080;
    callme::server s;
    std::atomic_bool is_running_;
    MockDummy md;
};

TEST_F(client_test, instantiation) {
    callme::client client("127.0.0.1", test_port);
}

TEST_F(client_test, call) {
    EXPECT_CALL(md, dummy_void_zeroarg());
    EXPECT_CALL(md, dummy_void_singlearg(5));
    EXPECT_CALL(md, dummy_void_multiarg(5, 6));
    callme::client client("127.0.0.1", test_port);
    client.call("dummy_void_zeroarg");
    client.call("dummy_void_singlearg", 5);
    client.call("dummy_void_multiarg", 5, 6);
}
