cmake_minimum_required(VERSION 3.0.0)
project(calculator)

find_package(rpclib REQUIRED)

include_directories(${RPCLIB_INCLUDE_DIR})

add_executable(calculator_server calculator_server.cc)
target_link_libraries(calculator_server ${RPCLIB_LIBS})
set_target_properties(
        calculator_server
        PROPERTIES
        CXX_STANDARD 14
        COMPILE_FLAGS "${CMAKE_CXX_FLAGS} ${RPCLIB_EXTRA_FLAGS}")

#add_executable(calculator_client calculator_client.cc)
#target_link_libraries(calculator_client ${RPCLIB_LIBS})
#set_property(TARGET calculator_client PROPERTY CXX_STANDARD 14)

