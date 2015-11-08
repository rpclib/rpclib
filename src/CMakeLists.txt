set(OUTPUT_LIBRARY_NAME ${CMAKE_PROJECT_NAME})

add_library(${OUTPUT_LIBRARY_NAME}
    callme/server.cc
    callme/tcp_server.cc)

if(compdb)
    add_custom_command(TARGET ${OUTPUT_LIBRARY_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E
                       copy ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_BINARY_DIR}/../compile_commands.json)
endif()

set_property(TARGET ${OUTPUT_LIBRARY_NAME} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${OUTPUT_LIBRARY_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)

target_link_libraries(${OUTPUT_LIBRARY_NAME} boost_system)
