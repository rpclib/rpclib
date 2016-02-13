enable_testing()

set(TEST_PROJECT_NAME ${CMAKE_PROJECT_NAME}_test)

set(TEST_SOURCES
    ../dependencies/src/gmock-gtest-all.cc
    testmain.cc
    testutils.h
    callme/dispatcher_test.cc
    callme/client_test.cc
    callme/response_test.cc
    )

include_directories("${PROJECT_SOURCE_DIR}/tests")

add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})

cmake_policy(SET CMP0054 OLD) # silence quoted variables warning

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    target_link_libraries(${TEST_PROJECT_NAME}
        ${CMAKE_PROJECT_NAME})
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
    set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG")
	STRING(REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
else()
    if(logging)
        target_link_libraries(${TEST_PROJECT_NAME}
            ${CMAKE_PROJECT_NAME} pthread rt)
        set(CMAKE_EXE_LINKER_FLAGS -Wl,--no-as-needed "${CMAKE_EXE_LINKER_FLAGS}")
    else()
        target_link_libraries(${TEST_PROJECT_NAME}
            ${CMAKE_PROJECT_NAME} pthread)
    endif()
endif()
