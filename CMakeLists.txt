cmake_minimum_required(VERSION 3.0.0)
project(callme)

include(cmake/compiler_specific.cmake)

option(test "Build unit tests." ON) # Makes boolean 'test' available.
option(compdb "Generate compilation database. Useful for YCM." ON)
option(examples "Build examples." ON)

if(compdb)
    set(CMAKE_EXPORT_COMPILE_COMMANDS "ON") # for YCM
endif()

########################################

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(MsgPack REQUIRED)
find_package(libuv REQUIRED)
find_package(Boost REQUIRED COMPONENTS system regex)

include_directories(include)
add_subdirectory(lib)

if(test)
    add_subdirectory(tests)
endif()

if(examples)
    add_subdirectory(examples)
endif()
