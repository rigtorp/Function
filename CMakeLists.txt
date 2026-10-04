# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)
project(Function LANGUAGES CXX)

if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
  set(FUNCTION_IS_TOP_LEVEL ON)
else()
  set(FUNCTION_IS_TOP_LEVEL OFF)
endif()

option(FUNCTION_BUILD_EXAMPLES "Build the example" ${FUNCTION_IS_TOP_LEVEL})
option(FUNCTION_BUILD_BENCHMARKS "Build the benchmark" ${FUNCTION_IS_TOP_LEVEL})
option(FUNCTION_BUILD_TESTS "Build regression tests" ${FUNCTION_IS_TOP_LEVEL})
option(FUNCTION_INSTALL "Generate installation rules" ${FUNCTION_IS_TOP_LEVEL})

include(GNUInstallDirs)

add_library(Function INTERFACE)
add_library(Function::Function ALIAS Function)
target_compile_features(Function INTERFACE cxx_std_11)
target_include_directories(Function INTERFACE
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
  "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

function(function_add_executable target source)
  add_executable(${target} ${source})
  target_link_libraries(${target} PRIVATE Function::Function)
  set_target_properties(${target} PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  if(NOT DEFINED CMAKE_CXX_STANDARD)
    set_property(TARGET ${target} PROPERTY CXX_STANDARD 11)
  endif()
endfunction()

if(FUNCTION_BUILD_EXAMPLES)
  function_add_executable(example examples/example.cpp)
endif()

if(FUNCTION_BUILD_BENCHMARKS)
  function_add_executable(benchmark benchmarks/benchmark.cpp)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(benchmark PRIVATE -fno-devirtualize)
  endif()
endif()

if(FUNCTION_BUILD_TESTS)
  include(CTest)
  if(BUILD_TESTING)
    function_add_executable(function_test tests/test.cpp)
    add_test(NAME function_test COMMAND function_test)
    include(tests/CompileFailures.cmake)
  endif()
endif()

if(FUNCTION_INSTALL)
  include(CMakePackageConfigHelpers)

  configure_package_config_file(cmake/FunctionConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/FunctionConfig.cmake"
    INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Function")

  install(TARGETS Function EXPORT FunctionTargets)
  install(FILES include/rigtorp/Function.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/rigtorp")
  install(FILES LICENSE DESTINATION "${CMAKE_INSTALL_DATADIR}/licenses/Function")
  install(EXPORT FunctionTargets NAMESPACE Function::
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Function")
  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/FunctionConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/Function")
endif()
