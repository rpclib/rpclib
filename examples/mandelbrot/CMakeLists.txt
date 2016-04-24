cmake_minimum_required(VERSION 3.0.0)
project(mandelbrot)

# Normally, you would want to distribute Findcallme.cmake
# within your project directory; I wanted to avoid copies
# of this file here, so I'm just setting the CMake module path.
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../cmake")
find_package(callme REQUIRED)

include_directories(${CALLME_INCLUDE_DIR} ${SFML_INCLUDE_DIR})

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(SFML COMPONENTS graphics window system REQUIRED)

add_executable(mandelbrot_srv mandelbrot_srv.cc)
target_link_libraries(mandelbrot_srv ${CALLME_LIBS})
set_property(TARGET mandelbrot_srv PROPERTY CXX_STANDARD 14)

add_executable(mandelbrot_client mandelbrot_client.cc)
target_link_libraries(mandelbrot_client ${CALLME_LIBS} ${SFML_LIBRARIES})
set_property(TARGET mandelbrot_client PROPERTY CXX_STANDARD 14)

if (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
elseif (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
endif()
