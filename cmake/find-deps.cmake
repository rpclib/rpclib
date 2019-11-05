
# conan is optional
if(EXISTS ${CMAKE_BINARY_DIR}/conan_paths.cmake)
  include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
endif()


find_package(Boost REQUIRED system)
find_package(fmt REQUIRED)
find_package(rapidjson QUIET)
find_package(gtest QUIET)


function(define_deps_macros)
  cmake_parse_arguments(MY "" "" "DEPS" ${ARGN})
  foreach(dep ${MY_DEPS})
    if(TARGET ${dep}::${dep})
      string(TOUPPER ${dep} dep_upper)
      set(RPCLIB_HAVE_${dep_upper} 1 PARENT_SCOPE)
      list(APPEND opt_deps ${dep})
    endif()
  endforeach()
endfunction()

define_deps_macros(DEPS rapidjson catch2)

configure_file(${PROJECT_SOURCE_DIR}/include/rpc/config.hpp.in
               ${CMAKE_BINARY_DIR}/include/rpc/config.hpp @ONLY)
