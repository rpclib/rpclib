set(SERVER_EXAMPLE_NAME server)
set(CLIENT_EXAMPLE_NAME client)

add_executable(${SERVER_EXAMPLE_NAME} server/main.cc)
add_executable(${CLIENT_EXAMPLE_NAME} client/main.cc)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    target_link_libraries(${SERVER_EXAMPLE_NAME} callme uv)
    target_link_libraries(${CLIENT_EXAMPLE_NAME} callme uv)
else()
    target_link_libraries(${SERVER_EXAMPLE_NAME} callme uv pthread)
    target_link_libraries(${CLIENT_EXAMPLE_NAME} callme uv pthread)
endif()
