# Compiler-specific options

cmake_policy(SET CMP0054 NEW) # silence quoted variables warning

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++14")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++14")
    if(coverage)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g --coverage -O0")
    endif()
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "Intel")
  # using Intel C++
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    add_definitions(-DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS -D_GNU_SOURCE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG /OPT:REF /OPT:ICF")
	STRING(REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
endif()
