cmake_minimum_required(VERSION 2.8.7)
project(callme)
include(cmake/compiler_specific.cmake)
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

################################################################################
#
# Options
#
################################################################################

option(CALLME_BUILD_TESTS "Build unit CALLME_BUILD_TESTSs." OFF) 
option(CALLME_GENERATE_COMPDB "Generate compilation database. Useful for YCM." OFF)
option(CALLME_BUILD_EXAMPLES "Build examples." ON)
option(CALLME_ENABLE_LOGGING "ALlow logging in the library for debug purposes. Also usable in a release build." OFF)
option(CALLME_ENABLE_COVERAGE "Generate CALLME_ENABLE_COVERAGE information" OFF)

################################################################################

add_definitions(-DASIO_STANDALONE)
add_definitions(-DCALLME_ASIO=clmdep_asio)
add_definitions(-DCALLME_FMT=clmdep_fmt)
if(CALLME_ENABLE_LOGGING)
    add_definitions(-DCALLME_ENABLE_LOGGING)
endif()

include_directories(include dependencies/include)

file(GLOB_RECURSE CALLME_HEADERS include/callme/*.h)
file(GLOB_RECURSE DEP_HEADERS 
    dependencies/include/*.h 
    dependencies/include/*.hpp)

set(OUTPUT_LIBRARY_NAME ${CMAKE_PROJECT_NAME})
set(DEP_SOURCES
    dependencies/src/format.cc
    dependencies/src/posix.cc)

add_library(${OUTPUT_LIBRARY_NAME}
    lib/callme/dispatcher.cc
    lib/callme/server.cc
    lib/callme/response.cc
    lib/callme/client.cc
    lib/callme/detail/server_session.cc
    ${DEP_SOURCES}
    ${DEP_HEADERS}
    ${CALLME_HEADERS})

target_link_libraries(${OUTPUT_LIBRARY_NAME})

if(CALLME_GENERATE_COMPDB)
    set(CMAKE_EXPORT_COMPILE_COMMANDS "ON") # for YCM
    add_custom_command(TARGET ${OUTPUT_LIBRARY_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E
                       copy ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_BINARY_DIR}/../compile_commands.json)
endif()

################################################################################

if(CALLME_BUILD_TESTS)
    set(TEST_PROJECT_NAME ${CMAKE_PROJECT_NAME}_test)
    set(TEST_SOURCES
        dependencies/src/gmock-gtest-all.cc
        tests/testmain.cc
        tests/testutils.h
        tests/callme/dispatcher_test.cc
        tests/callme/client_test.cc
        tests/callme/response_test.cc
        tests/callme/server_test.cc)

    include_directories("${PROJECT_SOURCE_DIR}/tests")

    add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})

    LinkTests()
endif()

################################################################################

if(CALLME_BUILD_EXAMPLES)
    set(SERVER_EXAMPLE_NAME server)
    set(CLIENT_EXAMPLE_NAME client)
    set(CUSTOMT_EXAMPLE_NAME custom_types)

    add_executable(${SERVER_EXAMPLE_NAME} examples/server/main.cc)
    add_executable(${CLIENT_EXAMPLE_NAME} examples/client/main.cc)
    add_executable(${CUSTOMT_EXAMPLE_NAME} examples/custom_types/main.cc)

    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
        target_link_libraries(${SERVER_EXAMPLE_NAME} callme)
        target_link_libraries(${CLIENT_EXAMPLE_NAME} callme)
        target_link_libraries(${CUSTOMT_EXAMPLE_NAME} callme)
    else()
        target_link_libraries(${SERVER_EXAMPLE_NAME} callme pthread)
        target_link_libraries(${CLIENT_EXAMPLE_NAME} callme pthread)
        target_link_libraries(${CUSTOMT_EXAMPLE_NAME} callme pthread)
    endif()
endif()
