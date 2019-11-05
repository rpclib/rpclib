#include "gtest/gtest.h"

#include "rpc/serializers/json_serializer.hpp"

TEST(JsonSerializerTests, call_to_json) {
  EXPECT_EQ(rpc::json_serializer::call_to_str("hello"),
            "{\"jsonrpc\":\"2.0\",\"method\":\"hello\",\"params\":[]}");

  EXPECT_EQ(rpc::json_serializer::call_to_str("hello", 1, 2),
            "{\"jsonrpc\":\"2.0\",\"method\":\"hello\",\"params\":[1,2]}");
}

TEST(JsonSerializerTests, call_t_with_id) {
  rpc::call_t call("hello", 43, 1, 2);
  EXPECT_EQ(
      rpc::json_serializer::call_to_str(call),
      "{\"jsonrpc\":\"2.0\",\"method\":\"hello\",\"id\":43,\"params\":[1,2]}");
}

TEST(JsonSerializerTests, call_t_without_id) {
  rpc::call_t call("hello", std::nullopt, 1, 2);
  EXPECT_EQ(
      rpc::json_serializer::call_to_str(call),
      "{\"jsonrpc\":\"2.0\",\"method\":\"hello\",\"params\":[1,2]}");
}
