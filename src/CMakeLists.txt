
add_library(rpc)

set_target_properties(rpc PROPERTIES CXX_STANDARD 17)

target_include_directories(rpc
    PUBLIC
      $<INSTALL_INTERFACE:include>
      $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
      $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    PRIVATE
      ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(rpc PUBLIC
  Boost::Boost
  $<TARGET_NAME_IF_EXISTS:rapidjson::rapidjson>)


target_sources(
  rpc PRIVATE rpc/serializers/json_serializer.cpp
)
