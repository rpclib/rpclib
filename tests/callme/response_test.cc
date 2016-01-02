#include "gtest/gtest.h"

#include <memory>

#include "callme/response.h"
#include "testutils.h"

using namespace callme::testutils;

TEST(response, object_ctor) {
    callme::response r(make_obj(3, 42, "foo", "bar"));
    EXPECT_EQ(r.get_id(), 42);
    EXPECT_TRUE(r.get_error() == "foo");

    std::string result;
    r.get_result().convert(&result);
    EXPECT_TRUE(result == "bar");
}
