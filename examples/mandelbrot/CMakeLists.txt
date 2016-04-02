cmake_minimum_required(VERSION 3.0.0)
project(mandelbrot)

set(CALLME_BASE_DIR "${PROJECT_SOURCE_DIR}/../..")
set(CMAKE_MODULE_PATH "${CALLME_BASE_DIR}/cmake")
include(compiler_specific)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(SFML COMPONENTS graphics window system)

add_executable(mandelbrot_srv mandelbrot_srv.cc)
add_executable(mandelbrot_client mandelbrot_client.cc)
target_link_libraries(mandelbrot_srv callme)
target_link_libraries(mandelbrot_client callme sfml-window sfml-system sfml-graphics)

