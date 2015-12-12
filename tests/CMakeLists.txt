enable_testing()

set(TEST_PROJECT_NAME ${CMAKE_PROJECT_NAME}_test)

set(TEST_SOURCES
    ../dependencies/src/gmock-gtest-all.cc
    testmain.cc
    callme/dispatcher_test.cc)

add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})

set_property(TARGET ${TEST_PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${TEST_PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)

target_link_libraries(${TEST_PROJECT_NAME}
    ${CMAKE_PROJECT_NAME} uv pthread)
