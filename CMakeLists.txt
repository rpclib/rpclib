cmake_minimum_required(VERSION 3.0.0)
project(callme)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

################################################################################
#
# Options
#
################################################################################

option(CALLME_BUILD_TESTS "Build unit CALLME_BUILD_TESTSs." OFF) 
option(CALLME_GENERATE_COMPDB "Generate compilation database. Useful for YCM." OFF)
option(CALLME_BUILD_EXAMPLES "Build examples." ON)
option(CALLME_ENABLE_LOGGING "ALlow logging in the library for debug purposes. Also usable in a release build." OFF)
option(CALLME_ENABLE_COVERAGE "Generate coverage information" OFF)
option(CALLME_INSTALL_MSGPACK "Install msgpack headers. Callme ships some minor patches with msgpack, so this is recommended!" ON)
option(CALLME_FORCE_M64 "Force -m64 in CXXFLAGS" OFF)
option(CALLME_FORCE_M32 "Force -m32 in CXXFLAGS" OFF)

################################################################################
#
# Compile & install the library
#
################################################################################

include(compiler_specific)
include(TargetArch)

set(CALLME_VERSION_MAJOR 1)
set(CALLME_VERSION_MINOR 0)
set(CALLME_VERSION_BUILD 1)

target_architecture(TARGET_ARCH)

if(CALLME_FORCE_M32)
    message("Compiling for 32-bit")
    add_definitions(-DCALLME_ARCH_X86)
    set(CALLME_TARGET_ARCH "x86")
elseif(CALLME_FORCE_M64)
    message("Compiling for 64-bit")
    add_definitions(-DCALLME_ARCH_X64)
    set(CALLME_TARGET_ARCH "x64")
elseif (${TARGET_ARCH} STREQUAL "i386")
    message("Compiling for 32-bit")
    add_definitions(-DCALLME_ARCH_X86)
    set(CALLME_TARGET_ARCH "x86")
elseif(${TARGET_ARCH} STREQUAL "x86_64")
    message("Compiling for 64-bit")
    add_definitions(-DCALLME_ARCH_X64)
    set(CALLME_TARGET_ARCH "x64")
endif()

add_definitions(-DASIO_STANDALONE)
add_definitions(-DCALLME_ASIO=clmdep_asio)
add_definitions(-DCALLME_FMT=clmdep_fmt)
if(CALLME_ENABLE_LOGGING)
    add_definitions(-DCALLME_ENABLE_LOGGING)
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output/bin)

configure_file(
  "${PROJECT_SOURCE_DIR}/include/callme/version.h.in"
  "${PROJECT_BINARY_DIR}/version.h")

include_directories(include)
include_directories(SYSTEM dependencies/include)

file(GLOB_RECURSE CALLME_HEADERS 
    include/callme/*.h
    include/msgpack/*.hpp)
file(GLOB_RECURSE DEP_HEADERS 
    dependencies/include/*.h 
    dependencies/include/*.hpp)

set(OUTPUT_LIBRARY_NAME ${CMAKE_PROJECT_NAME})
set(DEP_SOURCES
    dependencies/src/format.cc
    dependencies/src/posix.cc)

add_library(${OUTPUT_LIBRARY_NAME}
    lib/callme/dispatcher.cc
    lib/callme/server.cc
    lib/callme/response.cc
    lib/callme/client.cc
    lib/callme/detail/server_session.cc
    ${DEP_SOURCES}
    ${DEP_HEADERS}
    ${CALLME_HEADERS})

target_link_libraries(${OUTPUT_LIBRARY_NAME})

install(TARGETS ${OUTPUT_LIBRARY_NAME} DESTINATION lib)
install(DIRECTORY include/callme 
    DESTINATION include
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.in" EXCLUDE)
install(FILES ${PROJECT_BINARY_DIR}/version.h DESTINATION include/callme)

if (CALLME_INSTALL_MSGPACK)
    install(DIRECTORY include/msgpack DESTINATION include)
endif()

if(CALLME_GENERATE_COMPDB)
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

if(CALLME_BUILD_TESTS)
    set(TEST_PROJECT_NAME ${CMAKE_PROJECT_NAME}_test)
    set(TEST_SOURCES
        dependencies/src/gmock-gtest-all.cc
        tests/testmain.cc
        tests/testutils.h
        tests/callme/dispatcher_test.cc
        tests/callme/client_test.cc
        tests/callme/response_test.cc
        tests/callme/server_test.cc)

    include_directories(SYSTEM "${PROJECT_SOURCE_DIR}/tests")

    add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})
    SetLessStrictWarnings(${TEST_PROJECT_NAME})

    LinkTests()
endif()

################################################################################
#
# Example programs
#
################################################################################

if(CALLME_BUILD_EXAMPLES)
    set(SERVER_EXAMPLE_NAME server)
    set(CLIENT_EXAMPLE_NAME client)
    set(CUSTOMT_EXAMPLE_NAME custom_types)

    add_executable(${SERVER_EXAMPLE_NAME} examples/server/main.cc)
    add_executable(${CLIENT_EXAMPLE_NAME} examples/client/main.cc)
    add_executable(${CUSTOMT_EXAMPLE_NAME} examples/custom_types/main.cc)

    SetLessStrictWarnings(${SERVER_EXAMPLE_NAME})
    SetLessStrictWarnings(${CLIENT_EXAMPLE_NAME})
    SetLessStrictWarnings(${CUSTOMT_EXAMPLE_NAME})
    target_link_libraries(${SERVER_EXAMPLE_NAME} callme)
    target_link_libraries(${CLIENT_EXAMPLE_NAME} callme)
    target_link_libraries(${CUSTOMT_EXAMPLE_NAME} callme)
endif()


################################################################################
#
# CPack
#
################################################################################

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Modern msgpack-rpc for C++")
set(CPACK_PACKAGE_VENDOR "Tamás Szelei")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.md")
set(CPACK_PACKAGE_VERSION_MAJOR "${CALLME_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${CALLME_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_BUILD "${CALLME_VERSION_MINOR}")
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/output/pkg")
set(CPACK_PACKAGE_EXECUTABLES "libcallme" "callme")
set(CPACK_PACKAGE_NAME "lib${CMAKE_PROJECT_NAME}_${CALLME_TARGET_ARCH}")
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CALLME_TARGET_ARCH})
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Tamás Szelei")
set(CPACK_DEBIAN_PACKAGE_CONTACT "Tamás Szelei")
if(WIN32)
    set(CPACK_PACKAGE_NAME "${CMAKE_PROJECT_NAME}")
endif()

if(WIN32)
    #set(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
    #set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "libcallme ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
    set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} callme")
    #set(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.my-project-home-page.org")
    set(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\github.com/sztomi/callme")
    #set(CPACK_NSIS_CONTACT "me@my-personal-home-page.com")
    set(CPACK_NSIS_MODIFY_PATH ON)
endif()
include(CPack)
