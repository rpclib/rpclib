# Find the msgpack library
# http://msgpack.org

include(CheckCXXSourceCompiles)

find_library(
    MSGPACK_LIBRARIES
    NAMES msgpack libmsgpack
    HINTS ${MSGPACK_LIBRARY_DIRS})

find_path(MSGPACK_INCLUDE_DIRS
    NAMES msgpack.hpp)

mark_as_advanced(MSGPACK_LIBRARY_DIR MSGPACK_INCLUDE_DIRS)

if(MSGPACK_FOUND)
    file(READ
        ${MSGPACK_INCLUDE_DIRS}/msgpack/version_master.h VERSION_FILE_CONTENTS)

    string(REGEX REPLACE ";" "\\\\;" VERSION_FILE_CONTENTS "${VERSION_FILE_CONTENTS}")
    string(REGEX REPLACE "\n" ";" VERSION_FILE_CONTENTS "${VERSION_FILE_CONTENTS}")

    list(GET VERSION_FILE_CONTENTS 0 MAJOR_VER_LINE)
    list(GET VERSION_FILE_CONTENTS 1 MINOR_VER_LINE)
    list(GET VERSION_FILE_CONTENTS 2 REVISION_VER_LINE)
    string(REGEX REPLACE
        ".*MSGPACK_VERSION_MINOR.+([0-9]+)" "\\1"
        MSGPACK_VERSION_MINOR ${MINOR_VER_LINE})
    string(REGEX REPLACE
        ".*MSGPACK_VERSION_MAJOR.+([0-9]+)" "\\1"
        MSGPACK_VERSION_MAJOR ${MAJOR_VER_LINE})
    string(REGEX REPLACE
        ".*MSGPACK_VERSION_REVISION.+([0-9]+)" "\\1"
        MSGPACK_VERSION_REVISION ${REVISION_VER_LINE})
    # ... I'd rather eat a cockroach than work with cmake regex ever again
    # and cmake string function in general for that matter

    message(STATUS "MsgPack version: " ${MSGPACK_VERSION_MAJOR}.${MSGPACK_VERSION_MINOR}.${MSGPACK_VERSION_REVISION})


    set(CMAKE_REQUIRED_LIBRARIES ${MSGPACK_LIBRARIES})
    set(CMAKE_REQUIRED_INCLUDES ${MSGPACK_INCLUDE_DIRS})

    check_cxx_source_compiles("#include <msgpack.hpp>
    int main()
    {
        msgpack::sbuffer buffer;
        return 0;
    }" HAVE_MSGPACK_SUPPORT)

    unset(CMAKE_REQUIRED_LIBRARIES)
    unset(CMAKE_REQUIRED_INCLUDES)
endif()
