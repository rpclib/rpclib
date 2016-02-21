cmake_policy(SET CMP0054 OLD) # silence quoted variables warning

set(SERVER_EXAMPLE_NAME server)
set(CLIENT_EXAMPLE_NAME client)
set(CUSTOMT_EXAMPLE_NAME custom_types)

add_executable(${SERVER_EXAMPLE_NAME} server/main.cc)
add_executable(${CLIENT_EXAMPLE_NAME} client/main.cc)
add_executable(${CUSTOMT_EXAMPLE_NAME} custom_types/main.cc)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    target_link_libraries(${SERVER_EXAMPLE_NAME} callme)
    target_link_libraries(${CLIENT_EXAMPLE_NAME} callme)
    target_link_libraries(${CUSTOMT_EXAMPLE_NAME} callme)
else()
    target_link_libraries(${SERVER_EXAMPLE_NAME} callme pthread)
    target_link_libraries(${CLIENT_EXAMPLE_NAME} callme pthread)
    target_link_libraries(${CUSTOMT_EXAMPLE_NAME} callme pthread)
endif()
