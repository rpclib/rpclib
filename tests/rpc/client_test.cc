#include "gtest/gtest.h"

#include "rpc/client.h"
#include "rpc/server.h"
#include "testutils.h"

#include <chrono>

using namespace rpc::testutils;
using namespace std::literals::chrono_literals;

class client_test : public testing::Test {
public:
    client_test() : s("127.0.0.1", test_port), is_running_(false) {
        s.bind("dummy_void_zeroarg", [this]() { md.dummy_void_zeroarg(); });
        s.bind("dummy_void_singlearg",
               [this](int x) { md.dummy_void_singlearg(x); });
        s.bind("dummy_void_multiarg",
               [this](int x, int y) { md.dummy_void_multiarg(x, y); });
        s.bind("large_return", [](std::size_t bytes){ get_blob(bytes); });
        s.async_run();
    }

protected:
    static const int test_port = 8080;
    MockDummy md;
    rpc::server s;
    std::atomic_bool is_running_;
};

TEST_F(client_test, instantiation) {
    rpc::client client("127.0.0.1", test_port);
}

TEST_F(client_test, call) {
    EXPECT_CALL(md, dummy_void_zeroarg());
    EXPECT_CALL(md, dummy_void_singlearg(5));
    EXPECT_CALL(md, dummy_void_multiarg(5, 6));
    rpc::client client("127.0.0.1", test_port);
    client.call("dummy_void_zeroarg");
    client.call("dummy_void_singlearg", 5);
    client.call("dummy_void_multiarg", 5, 6);
}

TEST_F(client_test, notification) {
    EXPECT_CALL(md, dummy_void_zeroarg());
    rpc::client client("127.0.0.1", test_port);
    client.send("dummy_void_zeroarg");
    client.wait_all_responses();
    std::this_thread::sleep_for(50ms);
}

TEST_F(client_test, large_return) {
    rpc::client client("127.0.0.1", test_port);
    std::size_t blob_size = 2 << 10 << 10;
    for (int i = 0; i < 4; ++i) {
        client.call("large_return", blob_size);
        blob_size *= 2;
    }
    // no crash is enough
}
