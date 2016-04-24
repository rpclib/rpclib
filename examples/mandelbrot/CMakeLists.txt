cmake_minimum_required(VERSION 3.0.0)
project(mandelbrot)

# Normally, you would want to distribute Findcallme.cmake
# within your project directory; I wanted to avoid copies
# of this file here, so I'm just setting the CMake module path.
# In your project, you don't need the next line:
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/../../cmake")
find_package(callme REQUIRED)

# .. and probably only this one (if any):
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
find_package(SFML COMPONENTS graphics window system REQUIRED)

include_directories(${CALLME_INCLUDE_DIR} ${SFML_INCLUDE_DIR})

add_executable(mandelbrot_server mandelbrot_server.cc)
target_link_libraries(mandelbrot_srv ${CALLME_LIBS})
set_property(TARGET mandelbrot_srv PROPERTY CXX_STANDARD 14)

add_executable(mandelbrot_client mandelbrot_client.cc)
target_link_libraries(mandelbrot_client ${CALLME_LIBS} ${SFML_LIBRARIES})
set_property(TARGET mandelbrot_client PROPERTY CXX_STANDARD 14)
