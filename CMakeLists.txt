cmake_minimum_required(VERSION 3.0.0)
project(rpc)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

################################################################################
#
# Options
#
################################################################################

option(RPCLIB_BUILD_TESTS "Build unit RPCLIB_BUILD_TESTS." OFF)
option(RPCLIB_GENERATE_COMPDB "Generate compilation database. Useful for YCM." OFF)
option(RPCLIB_BUILD_EXAMPLES "Build examples." OFF)
option(RPCLIB_ENABLE_LOGGING "ALlow logging in the library for debug purposes. Also usable in a release build." OFF)
option(RPCLIB_ENABLE_COVERAGE "Generate coverage information" OFF)
option(RPCLIB_FORCE_M64 "Force -m64 in CXXFLAGS" OFF)
option(RPCLIB_FORCE_M32 "Force -m32 in CXXFLAGS" OFF)
option(RPCLIB_MSVC_STATIC_RUNTIME "MSVC only: build with /MT instead of /MD" OFF)

################################################################################
#
# Other configuration values
#
################################################################################

set(RPCLIB_DEFAULT_PORT 8080 CACHE STRING "Default port used for running tests and examples")
set(RPCLIB_CXX_STANDARD 11 CACHE STRING "C++ version used to build rpclib (Currently: Only 11 and 14 supported)")

if(NOT ${RPCLIB_CXX_STANDARD} EQUAL 14 AND NOT ${RPCLIB_CXX_STANDARD} EQUAL 11)
    message(fatal_error "Unsupported C++ standard: ${RPCLIB_CXX_STANDARD}")
endif()

################################################################################
#
# Compile & install the library
#
################################################################################

include(TargetArch)

set(RPCLIB_VERSION_MAJOR 1)
set(RPCLIB_VERSION_MINOR 0)
set(RPCLIB_VERSION_PATCH 0)

target_architecture(TARGET_ARCH)

if(RPCLIB_FORCE_M32)
    message(STATUS "Compiling for 32-bit")
    set(RPCLIB_ARCH_DEF "RPCLIB_ARCH_X86")
    set(RPCLIB_TARGET_ARCH "x86")
    set(RPCLIB_DEB_ARCH "i386")
elseif(RPCLIB_FORCE_M64)
    message(STATUS "Compiling for 64-bit")
    set(RPCLIB_ARCH_DEF "RPCLIB_ARCH_X64")
    set(RPCLIB_TARGET_ARCH "x64")
    set(RPCLIB_DEB_ARCH "amd64")
elseif (${TARGET_ARCH} STREQUAL "i386")
    message(STATUS "Compiling for 32-bit")
    set(RPCLIB_ARCH_DEF "RPCLIB_ARCH_X86")
    set(RPCLIB_TARGET_ARCH "x86")
    set(RPCLIB_DEB_ARCH "i386")
elseif(${TARGET_ARCH} STREQUAL "x86_64")
    message(STATUS "Compiling for 64-bit")
    set(RPCLIB_ARCH_DEF "RPCLIB_ARCH_X64")
    set(RPCLIB_TARGET_ARCH "x64")
    set(RPCLIB_DEB_ARCH "amd64")
endif()

if (WIN32)
    set(RPCLIB_OS_DEF "RPCLIB_WIN32")
elseif (LINUX)
    set(RPCLIB_OS_DEF "RPCLIB_LINUX")
elseif (APPLE)
    set(RPCLIB_OS_DEF "RPCLIB_MAC")
endif()

# This function sets the rpclib-specific flags that are used for
# development. You do not need this in your project. Instead,
# Findrpclib.cmake supplies a (possibly empty) RPCLIB_EXTRA_FLAGS variable
# that you should append to your build flags.
function(set_rpclib_flags TARGET)
    # clang is the compiler used for developing mainly, so
    # this is where I set the highest warning level
    # but feel free to add similar flags to GCC
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")

        set(RPCLIB_BUILD_FLAGS
            "-Wall -pedantic -Weverything -Wno-c++98-compat\
            -Wno-c++98-compat-pedantic -Wno-padded -Wno-missing-prototypes\
            -pthread")
        if(RPCLIB_CXX_STANDARD EQUAL 14)
            set(RPCLIB_BUILD_FLAGS "${RPCLIB_BUILD_FLAGS} -std=c++14")
        elseif(RPCLIB_CXX_STANDARD EQUAL 11)
            set(RPCLIB_BUILD_FLAGS "${RPCLIB_BUILD_FLAGS} -std=c++11")
        endif()

        if(RPCLIB_FORCE_M32)
            set(RPCLIB_BUILD_FLAGS "${RPCLIB_BUILD_FLAGS} -m32")
        elseif(RPCLIB_FORCE_M64)
            set(RPCLIB_BUILD_FLAGS "${RPCLIB_BUILD_FLAGS} -m64")
        endif()

        set(RPCLIB_DEP_LIBRARIES "pthread")

        if (RPCLIB_ENABLE_LOGGING)
            set(RPCLIB_DEP_LIBRARIES "${RPCLIB_DEP_LIBRARIES}")
        endif()

    elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")

        set(RPCLIB_BUILD_FLAGS "-Wall -pedantic -pthread")
        if(RPCLIB_CXX_STANDARD EQUAL 14)
            set(RPCLIB_BUILD_FLAGS "${RPCLIB_BUILD_FLAGS} -std=c++14")
        elseif(RPCLIB_CXX_STANDARD EQUAL 11)
            set(RPCLIB_BUILD_FLAGS "${RPCLIB_BUILD_FLAGS} -std=c++11")
        endif()

        if(RPCLIB_ENABLE_COVERAGE)
            set(RPCLIB_BUILD_FLAGS "${RPCLIB_BUILD_FLAGS} -g --coverage -O0")
        endif()

        if(RPCLIB_FORCE_M32)
            set(RPCLIB_BUILD_FLAGS "${RPCLIB_BUILD_FLAGS} -m32")
        elseif(RPCLIB_FORCE_M64)
            set(RPCLIB_BUILD_FLAGS "${RPCLIB_BUILD_FLAGS} -m64")
        endif()

        set(RPCLIB_DEP_LIBRARIES "pthread")

        if (RPCLIB_ENABLE_LOGGING)
            set(RPCLIB_DEP_LIBRARIES "${RPCLIB_DEP_LIBRARIES}")
        endif()

    elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")

        set(RPCLIB_COMPILE_DEFINITIONS
            "${RPCLIB_COMPILE_DEFINITIONS}"
            "WIN32_LEAN_AND_MEAN"
            "NOMINMAX"
            "VC_EXTRALEAN"
            "_CRT_SECURE_NO_WARNINGS"
            "_CRT_NONSTDC_NO_DEPRECATE"
            "_WIN32_WINNT=0x0501"
            "_GNU_SOURCE"
            "ASIO_HAS_STD_ADDRESSOF"
            "ASIO_HAS_STD_ARRAY"
            "ASIO_HAS_CSTDINT"
            "ASIO_HAS_STD_SHARED_PTR"
            "ASIO_HAS_STD_TYPE_TRAITS")

    endif()

    if (RPCLIB_EXTRA_BUILD_FLAGS)
        set(RPCLIB_BUILD_FLAGS "${RPCLIB_BUILD_FLAGS} ${RPCLIB_EXTRA_BUILD_FLAGS}")
    endif()

    set(RPCLIB_COMPILE_DEFINITIONS
        "${RPCLIB_COMPILE_DEFINITIONS}"
        "${RPCLIB_ARCH_DEF}"
        "${RPCLIB_OS_DEF}"
        "ASIO_STANDALONE"
        "RPCLIB_ASIO=clmdep_asio"
        "RPCLIB_FMT=clmdep_fmt"
        "RPCLIB_MSGPACK=clmdep_msgpack"
        "RPCLIB_DEFAULT_PORT=${RPCLIB_DEFAULT_PORT}"
        "RPCLIB_CXX_STANDARD=${RPCLIB_CXX_STANDARD}"
        )

    if(RPCLIB_ENABLE_LOGGING)
        set(RPCLIB_COMPILE_DEFINITIONS
            "${RPCLIB_COMPILE_DEFINITIONS};RPCLIB_ENABLE_LOGGING")
    endif()

    if(RPCLIB_BUILD_FLAGS)
        set_target_properties(${TARGET}
                PROPERTIES
                COMPILE_FLAGS "${RPCLIB_BUILD_FLAGS}")
    endif()

    if(RPCLIB_COMPILE_DEFINITIONS)
        set_target_properties(${TARGET}
                PROPERTIES
                COMPILE_DEFINITIONS "${RPCLIB_COMPILE_DEFINITIONS}")
    endif()

    if(RPCLIB_ENABLE_COVERAGE)
        set_target_properties(${TARGET}
                PROPERTIES
                LINK_FLAGS "-lgcov --coverage")
    endif()

    target_link_libraries(${TARGET} ${RPCLIB_DEP_LIBRARIES})

endfunction()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin)

configure_file(
    "${PROJECT_SOURCE_DIR}/include/rpc/version.h.in"
    "${PROJECT_BINARY_DIR}/version.h")

include_directories(include)
include_directories(include/rpc)
include_directories(SYSTEM dependencies/include)

file(GLOB_RECURSE RPCLIB_HEADERS
    include/rpc/*.h
    include/msgpack/*.hpp)
file(GLOB_RECURSE DEP_HEADERS
    dependencies/include/*.h
    dependencies/include/*.hpp)

if(RPCLIB_NAME_SUFFIX)
    set(OUTPUT_LIBRARY_NAME ${CMAKE_PROJECT_NAME}-${RPCLIB_NAME_SUFFIX})
else()
    set(OUTPUT_LIBRARY_NAME ${CMAKE_PROJECT_NAME})
endif()

set(DEP_SOURCES
    dependencies/src/format.cc
    dependencies/src/posix.cc)

add_library(${OUTPUT_LIBRARY_NAME}
    lib/rpc/dispatcher.cc
    lib/rpc/server.cc
    lib/rpc/client.cc
    lib/rpc/this_handler.cc
    lib/rpc/this_session.cc
    lib/rpc/this_server.cc
    lib/rpc/rpc_error.cc
    lib/rpc/detail/server_session.cc
    lib/rpc/detail/response.cc
    lib/rpc/detail/client_error.cc
    ${DEP_SOURCES}
    ${DEP_HEADERS}
    ${RPCLIB_HEADERS})

set_rpclib_flags(${OUTPUT_LIBRARY_NAME})

target_link_libraries(${OUTPUT_LIBRARY_NAME})

# When building via conan, respect the compilation settings.
if ("${CONAN_LINK_RUNTIME}" STREQUAL "/MT")
    set(RPCLIB_MSVC_STATIC_RUNTIME ON)
endif()

# MSVC static runtime support
#
# While this pollutes global flags (when using add_library), you would not want to
# build with a disparity anyway. (also, CMake still has no support for this, so you
# would end up doing something like this yourself).
if (RPCLIB_MSVC_STATIC_RUNTIME)
	# Set compiler options.
	set(variables
	  CMAKE_C_FLAGS_DEBUG
	  CMAKE_C_FLAGS_MINSIZEREL
	  CMAKE_C_FLAGS_RELEASE
	  CMAKE_C_FLAGS_RELWITHDEBINFO
	  CMAKE_CXX_FLAGS_DEBUG
	  CMAKE_CXX_FLAGS_MINSIZEREL
	  CMAKE_CXX_FLAGS_RELEASE
	  CMAKE_CXX_FLAGS_RELWITHDEBINFO
	)

	message(STATUS
		"MSVC -> forcing use of statically-linked runtime."
	)

	foreach(variable ${variables})
		if(${variable} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
		endif()
	endforeach()

endif()

install(TARGETS ${OUTPUT_LIBRARY_NAME} DESTINATION lib)
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
    PATTERN "*.inl"
    PATTERN "*.in" EXCLUDE)
install(FILES ${PROJECT_BINARY_DIR}/version.h DESTINATION include/rpc)

if(RPCLIB_GENERATE_COMPDB)
    set(CMAKE_EXPORT_COMPILE_COMMANDS "ON") # for YCM
    add_custom_command(TARGET ${OUTPUT_LIBRARY_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E
                       copy ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_BINARY_DIR}/../compile_commands.json)
endif()

################################################################################
#
# Unit tests
#
################################################################################

if(RPCLIB_BUILD_TESTS)
    set(TEST_PROJECT_NAME ${CMAKE_PROJECT_NAME}_test)
    set(TEST_SOURCES
        dependencies/src/gmock-gtest-all.cc
        tests/testmain.cc
        tests/testutils.h
        tests/rpc/dispatcher_test.cc
        tests/rpc/client_test.cc
        tests/rpc/response_test.cc
        tests/rpc/server_test.cc
        tests/rpc/this_handler_test.cc
        tests/rpc/this_session_test.cc
        tests/rpc/server_session_test.cc
        tests/rpc/this_server_test.cc)

    include_directories(SYSTEM "${PROJECT_SOURCE_DIR}/tests")

    add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})
    set_rpclib_flags(${TEST_PROJECT_NAME})
    target_link_libraries(
        ${TEST_PROJECT_NAME}
        ${OUTPUT_LIBRARY_NAME}
        ${RPCLIB_DEP_LIBRARIES})

    # Set less strict warning for tests, since google test is not quite
    # warning-clean
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        get_target_property(ORIGINAL_FLAGS ${TEST_PROJECT_NAME} COMPILE_FLAGS)
        set_target_properties(${TEST_PROJECT_NAME} PROPERTIES COMPILE_FLAGS
            "${ORIGINAL_FLAGS} -Wno-sign-conversion -Wno-weak-vtables -Wno-unused-member-function \
            -Wno-global-constructors -Wno-used-but-marked-unused -Wno-covered-switch-default \
            -Wno-missing-variable-declarations -Wno-deprecated -Wno-unused-macros -Wno-undef \
            -Wno-exit-time-destructors -Wno-switch-enum -Wno-format-nonliteral -Wno-unused-parameter -Wno-disabled-macro-expansion")
    endif()

endif()

################################################################################
#
# Example programs
#
################################################################################

if(RPCLIB_BUILD_EXAMPLES)
    set(RPCLIB_ROOT_DIR "${PROJECT_SOURCE_DIR}")
    set(RPCLIB_PROJECT_NAME "${CMAKE_PROJECT_NAME}")
    set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/localbuild")
    set(ENV{RPCLIB_DEFAULT_PORT} "${RPCLIB_DEFAULT_PORT}")
    #add_subdirectory(examples/server)
    #add_subdirectory(examples/client)
    add_subdirectory(examples/echo)
    add_subdirectory(examples/mandelbrot)
    add_subdirectory(examples/calculator)
endif()


################################################################################
#
# CPack
#
################################################################################

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern msgpack-rpc library for C++")
set(CPACK_PACKAGE_DESCRIPTION "rpclib is a modern msgpack-rpc library for C++. It allows connecting applications through a network channel where they can talk to each other using the msgpack-rpc protocol. It provides both a client and server, which can be used independently.")
set(CPACK_PACKAGE_VENDOR "Tamás Szelei")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
set(CPACK_PACKAGE_VERSION_MAJOR "${RPCLIB_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${RPCLIB_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${RPCLIB_VERSION_PATCH}")
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/output/pkg")
set(CPACK_PACKAGE_EXECUTABLES "librpc" "rpc")
set(CPACK_PACKAGE_NAME "lib${CMAKE_PROJECT_NAME}_${RPCLIB_DEB_ARCH}")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${RPCLIB_DEB_ARCH})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tamás Szelei")
set(CPACK_DEBIAN_PACKAGE_CONTACT "Tamás Szelei")
if(WIN32)
    set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
endif()

if(WIN32)
    #set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
    #set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "librpc ${RPCLIB_VERSION_MAJOR}.${RPCLIB_VERSION_MINOR}.${RPCLIB_VERSION_PATCH}")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} rpclib")
    #set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
    set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\github.com/sztomi/rpc")
    #set(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
    set(CPACK_NSIS_MODIFY_PATH ON)
endif()
include(CPack)
