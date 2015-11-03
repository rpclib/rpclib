cmake_minimum_required(VERSION 3.0.0)

include(cmake/compiler_specific.cmake)

option(test "Build unit tests." ON) # Makes boolean 'test' available.
option(compdb "Generate compilation database. Useful for YCM." ON)

########################################

if(compdb)
    set(CMAKE_EXPORT_COMPILE_COMMANDS "ON") # for YCM
endif()

project(callme)

include_directories(include)

add_subdirectory(src)

if(test)
    add_subdirectory(tests)
endif()
