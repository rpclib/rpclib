include(coverage)

add_executable(rpc-tests
  test_main.cpp
  rpc/serializers/json_serializer_test.cpp)

target_link_libraries(rpc-tests PRIVATE gtest::gtest rpc)

set_target_properties(rpc-tests PROPERTIES CXX_STANDARD 17)

if(RPCLIB_ENABLE_COVERAGE)
  enable_coverage(rpc-tests)
endif()
