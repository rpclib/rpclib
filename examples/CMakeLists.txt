set(SERVER_EXAMPLE_NAME server)
add_executable(${SERVER_EXAMPLE_NAME} server/main.cc)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    target_link_libraries(${SERVER_EXAMPLE_NAME} callme uv)
else()
    target_link_libraries(${SERVER_EXAMPLE_NAME} callme uv pthread)
endif()
