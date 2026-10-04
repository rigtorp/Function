cmake_minimum_required(VERSION 3.5)
project(Function)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fno-devirtualize)
endif()

add_executable(example example.cpp)
add_executable(benchmark benchmark.cpp)

include(CTest)
if(BUILD_TESTING)
  add_executable(function_test test.cpp)
  add_test(NAME function_test COMMAND function_test)
endif()
