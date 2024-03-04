include(coverage)

set(TEST_PROJECT_NAME ${CMAKE_PROJECT_NAME}_test)
set(TEST_SOURCES
  ${RPCLIB_DEPENDENCIES}/googletest/googletest/src/gtest-all.cc
  ${RPCLIB_DEPENDENCIES}/googletest/googlemock/src/gmock-all.cc
  testmain.cc
  testutils.h
  rpc/dispatcher_test.cc
  rpc/client_test.cc
  rpc/response_test.cc
  rpc/server_test.cc
  rpc/this_handler_test.cc
  rpc/this_session_test.cc
  rpc/server_session_test.cc
  rpc/this_server_test.cc)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})

target_include_directories(${TEST_PROJECT_NAME} 
  SYSTEM PRIVATE "${PROJECT_SOURCE_DIR}/tests"
  PRIVATE "${RPCLIB_DEPENDENCIES}/include"
   "${RPCLIB_DEPENDENCIES}/googletest/googletest/include"
   "${RPCLIB_DEPENDENCIES}/googletest/googlemock/include"
   "${RPCLIB_DEPENDENCIES}/googletest/googletest"
   "${RPCLIB_DEPENDENCIES}/googletest/googlemock"
   )

target_link_libraries(${TEST_PROJECT_NAME} ${PROJECT_NAME} Threads::Threads)

# Set less strict warning for tests, since google test is not quite
# warning-clean
if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
  get_target_property(ORIGINAL_FLAGS ${TEST_PROJECT_NAME} COMPILE_OPTION)
  target_compile_options(${TEST_PROJECT_NAME} PRIVATE -Wno-sign-conversion -Wno-weak-vtables -Wno-unused-member-function
    -Wno-global-constructors -Wno-used-but-marked-unused -Wno-covered-switch-default
    -Wno-missing-variable-declarations -Wno-deprecated -Wno-unused-macros -Wno-undef
    -Wno-exit-time-destructors -Wno-switch-enum -Wno-format-nonliteral -Wno-unused-parameter -Wno-disabled-macro-expansion)
endif()

if(RPCLIB_ENABLE_COVERAGE)
  enable_coverage(${TEST_PROJECT_NAME})
endif()
