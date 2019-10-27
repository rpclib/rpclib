cmake_minimum_required(VERSION 3.12)
project(rpc VERSION 3.0.0)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
include(find-deps)

if(TARGET catch2::catch2)
  add_subdirectory(test)
else()
  message(STATUS "NOTE: Not building tests because Catch2 was not found")
endif()
