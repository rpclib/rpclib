#include "gtest/gtest.h"

#include "rpc/client.h"
#include "rpc/server.h"
#include "rpc/rpc_error.h"
#include "testutils.h"
#include "format.h"

#include <chrono>
#include <thread>

using namespace rpc::testutils;

class client_test : public testing::Test {
public:
    client_test() : s("127.0.0.1", test_port), is_running_(false) {
        s.bind("dummy_void_zeroarg", [this]() { md.dummy_void_zeroarg(); });
        s.bind("dummy_void_singlearg",
               [this](int x) { md.dummy_void_singlearg(x); });
        s.bind("dummy_void_multiarg",
               [this](int x, int y) { md.dummy_void_multiarg(x, y); });
        s.bind("large_return", [](std::size_t bytes){ get_blob(bytes); });
        s.bind("sleep", [](uint64_t ms) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ms));
        });
        s.async_run();
    }

protected:
    static RPCLIB_CONSTEXPR uint16_t test_port = rpc::constants::DEFAULT_PORT;
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
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
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

TEST_F(client_test, timeout_setting_works) {
    rpc::client client("127.0.0.1", test_port);
    const uint64_t default_timeout = 5000;
    EXPECT_EQ(client.get_timeout(), default_timeout);

    const uint64_t short_timeout = 50;
    client.set_timeout(short_timeout);

    EXPECT_EQ(client.get_timeout(), short_timeout);
    EXPECT_THROW(client.call("sleep", short_timeout + 1), rpc::timeout);

    client.set_timeout(short_timeout * 2);
    EXPECT_EQ(client.get_timeout(), short_timeout * 2);
    EXPECT_NO_THROW(client.call("sleep", short_timeout + 1));
}

TEST_F(client_test, timeout_right_msg) {
    rpc::client client("127.0.0.1", test_port);
    const uint64_t short_timeout = 50;
    try {
        client.set_timeout(short_timeout);
        client.call("sleep", short_timeout + 10);
        FAIL() << "There was no exception thrown.";
    } catch (rpc::timeout &t) {
        auto expected_msg = RPCLIB_FMT::format(
            "rpc::timeout: Timeout of {}ms while calling RPC function '{}'",
            client.get_timeout(), "sleep");
        EXPECT_TRUE(str_match(t.what(), expected_msg));
    }
}
