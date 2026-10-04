# SPDX-License-Identifier: MIT
cmake_minimum_required(VERSION 3.20)
project(FunctionConsumer LANGUAGES CXX)

if(FUNCTION_TEST_INSTALLED)
  find_package(Function CONFIG REQUIRED)
else()
  add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../.." Function)
endif()

if(TARGET example OR TARGET benchmark OR TARGET function_test)
  message(FATAL_ERROR "Function enabled optional executables in a consumer")
endif()

add_executable(consumer "${CMAKE_CURRENT_LIST_DIR}/../../examples/example.cpp")
target_link_libraries(consumer PRIVATE Function::Function)
enable_testing()
add_test(NAME consumer COMMAND consumer)
