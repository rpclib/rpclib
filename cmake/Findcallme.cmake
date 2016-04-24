#
# Findcallme.cmake can be distributed with the source of 
# your software that depends on callme. This allows you
# to write 
#
#     find_package(callme) 
#
# in your CMakeLists.txt.
#
# The script sets the following variables:
#
#   * CALLME_FOUND: true if the callme headers and libraries were found
#   * CALLME_INCLUDE_DIR: the directory where callme includes are located. 
#                         This means #include "callme/server.h" works if 
#                         you add this directory, not #include "server.h".
#   * CALLME_LIBS: The static libraries of callme (for now, this is only one
#                  library, but plural was chosen to be future-proof).
#
# For finding in custom locations, you may set CALLME_ROOT as a cmake variable or
# environment variable.
#
# Greatly inspired by FindSFML.cmake 
# (https://github.com/SFML/SFML/blob/master/cmake/Modules/FindSFML.cmake)
#

set(FIND_CALLME_PATHS
    ${CALLME_ROOT}
    $ENV{CALLME_ROOT}
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local
    /usr
    /opt/local
    /opt)

find_path(CALLME_INCLUDE_DIR
    PATH_SUFFIXES "include"
    NAMES "callme/version.h")

find_library(CALLME_LIBS
    NAMES libcallme callme)

if(CALLME_INCLUDE_DIR)
    file(READ 
        "${CALLME_INCLUDE_DIR}/callme/version.h" 
        CALLME_VERSION_CONTENTS)
    string(REGEX REPLACE 
        ".*#define CALLME_VERSION_MAJOR ([0-9]+).*" "\\1" 
        CALLME_VERSION_MAJOR "${CALLME_VERSION_CONTENTS}")
    string(REGEX REPLACE 
        ".*#define CALLME_VERSION_MINOR ([0-9]+).*" "\\1" 
        CALLME_VERSION_MINOR "${CALLME_VERSION_CONTENTS}")
    string(REGEX REPLACE 
        ".*#define CALLME_VERSION_PATCH ([0-9]+).*" "\\1" 
        CALLME_VERSION_PATCH "${CALLME_VERSION_CONTENTS}")
    set(CALLME_VERSION_STR
        "${CALLME_VERSION_MAJOR}.${CALLME_VERSION_MINOR}.${CALLME_VERSION_PATCH}")
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    list(APPEND CMAKE_CXX_FLAGS "-pthread")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    list(APPEND CMAKE_CXX_FLAGS "-pthread")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    list(APPEND CMAKE_CXX_FLAGS "/EHsc")
    list(APPEND CMAKE_CXX_FLAGS_DEBUG "/Zi") 
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(callme 
                      FOUND_VAR CALLME_FOUND
                      REQUIRED_VARS CALLME_INCLUDE_DIR CALLME_LIBS
                      VERSION_VAR CALLME_VERSION_STRING)

