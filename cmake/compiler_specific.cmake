# Compiler-specific options

cmake_policy(SET CMP0054 OLD) # silence quoted variables warning

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CALLME_ORIGINAL_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS 
        "${CMAKE_CXX_FLAGS} -Wall -pedantic -Weverything -Wno-c++98-compat\
        -Wno-c++98-compat-pedantic -Wno-padded -Wno-missing-prototypes\
        -pthread -std=c++14 -fsanitize=thread")
    if(CALLME_FORCE_M32)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    endif()
    if(CALLME_FORCE_M64)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
    endif()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -pthread -std=c++14")
    if(CALLME_ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --coverage -O0")
    endif()
    if(CALLME_FORCE_M32)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
    endif()
    if(CALLME_FORCE_M64)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64")
    endif()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  # using Intel C++
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    add_definitions(
        -DWIN32_LEAN_AND_MEAN 
        -DNOMINMAX 
        -DVC_EXTRALEAN 
        -D_CRT_SECURE_NO_WARNINGS 
        -D_CRT_NONSTDC_NO_DEPRECATE 
        -D_WIN32_WINNT=0x0501 
        -D_GNU_SOURCE)

    add_definitions(
        -DASIO_HAS_STD_ADDRESSOF
        -DASIO_HAS_STD_ARRAY    
        -DASIO_HAS_CSTDINT
        -DASIO_HAS_STD_SHARED_PTR
        -DASIO_HAS_STD_TYPE_TRAITS)

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi") 
        set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
	STRING(REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
endif()

if (WIN32)
    add_definitions(-DCALLME_WIN32)
elseif (LINUX)
    add_definitions(-DCALLME_LINUX)
elseif (APPLE)
    add_definitions(-DCALLME_MAC)
endif()


function(LinkTests)
    if (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
        target_link_libraries(${TEST_PROJECT_NAME}
            ${CMAKE_PROJECT_NAME})
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
        set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "${CMAKE_SHARED_LINKER_FLAGS_DEBUG} /DEBUG")
        STRING(REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    else()
        if(CALLME_ENABLE_LOGGING)
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
