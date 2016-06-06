# Compiler-specific options

cmake_policy(SET CMP0054 OLD) # silence quoted variables warning


function(LinkTests)
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
        target_link_libraries(${TEST_PROJECT_NAME}
            ${CMAKE_PROJECT_NAME})
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG")
        string(REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    else()
        if(RPCLIB_ENABLE_LOGGING)
            target_link_libraries(${TEST_PROJECT_NAME}
                ${CMAKE_PROJECT_NAME} pthread rt)
        else()
            target_link_libraries(${TEST_PROJECT_NAME}
                ${CMAKE_PROJECT_NAME} pthread)
        endif()
    endif()
endfunction()


function(SetLessStrictWarnings TARGET)
    if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS 
            "-Wno-sign-conversion -Wno-weak-vtables -Wno-unused-member-function \
            -Wno-global-constructors -Wno-used-but-marked-unused -Wno-covered-switch-default \
            -Wno-missing-variable-declarations -Wno-deprecated -Wno-unused-macros -Wno-undef \
            -Wno-exit-time-destructors -Wno-switch-enum -Wno-format-nonliteral -Wno-unused-parameter -Wno-disabled-macro-expansion")
    endif()
endfunction()
