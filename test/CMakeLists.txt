
add_executable(rpc-tests
  base.cpp)

target_link_libraries(rpc-tests PRIVATE catch2::catch2)
set_target_properties(rpc-tests PROPERTIES CXX_STANDARD 17)
