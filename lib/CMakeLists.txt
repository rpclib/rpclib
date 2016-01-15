add_definitions(-DASIO_STANDALONE)

set(OUTPUT_LIBRARY_NAME ${CMAKE_PROJECT_NAME})

set(DEP_SOURCES
    ../dependencies/src/format.cc
    ../dependencies/src/posix.cc
    )

add_library(${OUTPUT_LIBRARY_NAME}
    callme/dispatcher.cc
    callme/server.cc
    callme/response.cc
    callme/client.cc
    callme/detail/log.cc
    callme/detail/server_session.cc
    ${DEP_SOURCES}
    ${DEP_HEADERS}
    ${CALLME_HEADERS})

if(compdb)
    add_custom_command(TARGET ${OUTPUT_LIBRARY_NAME} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E
                       copy ${CMAKE_BINARY_DIR}/compile_commands.json ${CMAKE_BINARY_DIR}/../compile_commands.json)
endif()

target_link_libraries(${OUTPUT_LIBRARY_NAME})
