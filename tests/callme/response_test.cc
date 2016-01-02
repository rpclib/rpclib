#include "gtest/gtest.h"

#include <memory>

#include "callme/response.h"
#include "testutils.h"

using namespace callme::testutils;

TEST(response, object_ctor) {
	auto o = make_obj(3, 42, "foo", "bar");
    callme::response r(o.get());
    EXPECT_EQ(r.get_id(), 42);
    EXPECT_TRUE(r.get_error() == "foo");

    std::string result;
    r.get_result().convert(&result);
    EXPECT_TRUE(result == "bar");
}
