#
# This is a "fake" Findrpclib.cmake. In your project, you will probably want
# to use the Findrpclib.cmake one directory level up from here.
#

set(RPCLIB_INCLUDE_DIR "${RPCLIB_ROOT_DIR}/include")

if(RPCLIB_INCLUDE_DIR)
    file(READ 
        "${RPCLIB_INCLUDE_DIR}/rpc/version.h" 
        RPCLIB_VERSION_CONTENTS)
    string(REGEX REPLACE 
        ".*#define RPCLIB_VERSION_MAJOR ([0-9]+).*" "\\1" 
        RPCLIB_VERSION_MAJOR "${RPCLIB_VERSION_CONTENTS}")
    string(REGEX REPLACE 
        ".*#define RPCLIB_VERSION_MINOR ([0-9]+).*" "\\1" 
        RPCLIB_VERSION_MINOR "${RPCLIB_VERSION_CONTENTS}")
    string(REGEX REPLACE 
        ".*#define RPCLIB_VERSION_PATCH ([0-9]+).*" "\\1" 
        RPCLIB_VERSION_PATCH "${RPCLIB_VERSION_CONTENTS}")
    set(RPCLIB_VERSION_STR
        "${RPCLIB_VERSION_MAJOR}.${RPCLIB_VERSION_MINOR}.${RPCLIB_VERSION_PATCH}")
endif()

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(RPCLIB_EXTRA_FLAGS "-pthread")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(RPCLIB_EXTRA_FLAGS "-pthread")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    set(RPCLIB_EXTRA_FLAGS "/EHsc")
    set(RPCLIB_EXTRA_FLAGS_DEBUG "/Zi")
endif()

set(RPCLIB_FOUND "1")
set(RPCLIB_LIBS "${RPCLIB_PROJECT_NAME}")
