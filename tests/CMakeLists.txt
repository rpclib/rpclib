enable_testing()

set(TEST_PROJECT_NAME ${CMAKE_PROJECT_NAME}_test)

set(TEST_SOURCES
    ../dependencies/src/gmock-gtest-all.cc
    testmain.cc
    callme/dispatcher_test.cc
    callme/client_test.cc)

add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    target_link_libraries(${TEST_PROJECT_NAME}
        ${CMAKE_PROJECT_NAME} uv)
else()
    target_link_libraries(${TEST_PROJECT_NAME}
        ${CMAKE_PROJECT_NAME} uv pthread)
endif()
