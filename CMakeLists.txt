cmake_minimum_required(VERSION 3.2)
project(Function)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(example example.cpp)
add_executable(benchmark benchmark.cpp)