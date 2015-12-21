# Compiler-specific options

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    if(coverage)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --coverage -O0")
    endif()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  # using Intel C++
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    add_definitions(-DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS -D_GNU_SOURCE)
endif()
