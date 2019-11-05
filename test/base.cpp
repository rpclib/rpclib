#define CATCH_CONFIG_MAIN



TEST_CASE( "boop", "[beep]" ) {
  REQUIRE( rpc::json_serializer::call_to_str("beep", 2, 3.14, 3.15, 5, "henlo") == "foo" );
}
