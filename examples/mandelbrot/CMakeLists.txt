cmake_minimum_required(VERSION 3.0.0)
project(mandelbrot)

# Find our dependencies
find_package(rpclib REQUIRED)
find_package(SFML COMPONENTS graphics window system REQUIRED)

include_directories(${RPCLIB_INCLUDE_DIR} ${SFML_INCLUDE_DIR})

add_executable(mandelbrot_server mandelbrot_server.cc)
target_link_libraries(mandelbrot_server ${RPCLIB_LIBS})
set_property(TARGET mandelbrot_server PROPERTY CXX_STANDARD 14)

add_executable(mandelbrot_client mandelbrot_client.cc)
target_link_libraries(mandelbrot_client ${RPCLIB_LIBS} ${SFML_LIBRARIES})
set_property(TARGET mandelbrot_client PROPERTY CXX_STANDARD 14)

