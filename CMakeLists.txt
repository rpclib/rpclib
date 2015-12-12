cmake_minimum_required(VERSION 2.8.7)
project(callme)

include(cmake/compiler_specific.cmake)

option(test "Build unit tests." ON) # Makes boolean 'test' available.
option(compdb "Generate compilation database. Useful for YCM." ON)
option(examples "Build examples." ON)
option(logging "ALlow logging in the library for debug purposes. Also usable in a release build." ON)

if(compdb)
    set(CMAKE_EXPORT_COMPILE_COMMANDS "ON") # for YCM
endif()

if(logging)
    add_definitions(-DCALLME_LOGGING)
endif()

########################################

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(Boost REQUIRED)

include_directories(include
    dependencies/include
    dependencies/libuv/libuv/include)
file(GLOB_RECURSE CALLME_HEADERS include/callme/*.h)
file(GLOB_RECURSE DEP_HEADERS dependencies/include/*.h dependencies/include/*.hpp)

add_subdirectory(lib)

if(test)
    add_subdirectory(tests)
endif()

if(examples)
    add_subdirectory(examples)
endif()
