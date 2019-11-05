cmake_minimum_required(VERSION 3.12)
project(rpc VERSION 3.0.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(find-deps)

option(RPCLIB_ENABLE_COVERAGE "Generate coverage information" OFF)

add_subdirectory(src)

if(TARGET gtest::gtest)
  add_subdirectory(test)
else()
  message(STATUS "NOTE: Not building tests because gtest was not found")
endif()
