enable_testing()

find_package(GTest)
find_package(gmock)

include_directories(${GTEST_INCLUDE_DIRS})

set(TEST_SOURCES
    callme/dispatcher_test.cc)

set(TEST_PROJECT_NAME ${CMAKE_PROJECT_NAME}_test)

add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})

set_property(TARGET ${TEST_PROJECT_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${TEST_PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)

target_link_libraries(${TEST_PROJECT_NAME}
    ${CMAKE_PROJECT_NAME} ${GMOCK_LIBRARIES}
    ${GTEST_BOTH_LIBRARIES} pthread)
