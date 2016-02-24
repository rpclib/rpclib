#include "gtest/gtest.h"

#include <memory>
#include <atomic>
#include <thread>
#include "testutils.h"

#include "callme/response.h"

using namespace callme::testutils;

TEST(response, object_ctor) {
    auto o = make_unpacked(3, 42, "foo", "bar");
    callme::response r(std::move(o));
    EXPECT_EQ(r.get_id(), 42);
    EXPECT_TRUE(r.get_error() == "foo");

    std::string result;
    r.get_result().convert(&result);
    EXPECT_TRUE(result == "bar");
}

TEST(response, writing) {
    auto obj = make_unpacked(1, 42, "foo", "bar");
    callme::response r(std::move(obj));
    auto buf1 = r.get_data();
    callme::response::response_type same_obj(1, 42, msgpack::object("foo"),
                                             msgpack::object("bar"));
    msgpack::sbuffer buf2;
    msgpack::pack(buf2, same_obj);

    EXPECT_EQ(buf1.size(), buf2.size());
    EXPECT_EQ(0, memcmp(buf2.data(), buf2.data(), buf1.size()));
}
