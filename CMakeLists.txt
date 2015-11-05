cmake_minimum_required(VERSION 3.0.0)
project(callme)

include(cmake/compiler_specific.cmake)

option(test "Build unit tests." ON) # Makes boolean 'test' available.
option(compdb "Generate compilation database. Useful for YCM." ON)

if(compdb)
    set(CMAKE_EXPORT_COMPILE_COMMANDS "ON") # for YCM
endif()

########################################

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(MsgPack REQUIRED)
find_package(Boost REQUIRED COMPONENTS system regex)

include_directories(include)
add_subdirectory(src)

if(test)
    add_subdirectory(tests)
endif()
