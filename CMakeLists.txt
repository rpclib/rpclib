cmake_minimum_required(VERSION 3.0.0)

option(test "Build unit tests." OFF) # Makes boolean 'test' available.
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
