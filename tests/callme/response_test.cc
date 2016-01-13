#include "gtest/gtest.h"

#include <memory>
#include <atomic>
#include <thread>

#include "callme/response.h"
#include "callme/detail/uv_adaptor.h"
#include "uv.h"
#include "testutils.h"

using namespace callme::testutils;

TEST(response, object_ctor) {
    auto o = make_unpacked(3, 42, "foo", "bar");
    callme::response r(o.get());
    EXPECT_EQ(r.get_id(), 42);
    EXPECT_TRUE(r.get_error() == "foo");

    std::string result;
    r.get_result().convert(&result);
    EXPECT_TRUE(result == "bar");
}

class response_write_test : public tcp_tester {};

TEST_F(response_write_test, writing) {
    start_server();
    start_client();
    run_test_loop();

    auto obj = make_unpacked(1, 42, "foo", "bar");
    callme::response r(obj.get());
    r.write(reinterpret_cast<uv_stream_t *>(client_));

    callme::response::response_type same_obj(1, 42, msgpack::object("foo"),
                                             msgpack::object("bar"));
    msgpack::sbuffer buf;
    msgpack::pack(buf, same_obj);

    while (!read_finished_) {
        // nop
    }

    EXPECT_EQ(buf.size(), read_buf_.size());
    EXPECT_EQ(0, memcmp(buf.data(), read_buf_.data(), buf.size()));
}
