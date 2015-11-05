enable_testing()
find_package(GTest)

include_directories(${GTEST_INCLUDE_DIRS})

set(TEST_SOURCES
    callme_test.cc)

set(TEST_PROJECT_NAME ${CMAKE_PROJECT_NAME}_test)

add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})

set_property(TARGET ${TEST_PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${TEST_PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)

message("${GTEST_BOTH_LIBRARIES}")

target_link_libraries(${TEST_PROJECT_NAME}
    ${CMAKE_PROJECT_NAME}
    ${GTEST_BOTH_LIBRARIES})
