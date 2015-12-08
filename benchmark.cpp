#include "Function.h"
#include <chrono>
#include <iostream>

int main(int argc, char *argv[]) {
  constexpr size_t count = 100000000;

  volatile size_t state = 0;

  using namespace std::chrono;
  {
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i) {
      std::function<void()> stdfun([&state, i]() { state = i; });
      stdfun();
    }
    auto stop = high_resolution_clock::now();
    auto duration = stop - start;
    std::cout << "std::function: "
              << duration_cast<nanoseconds>(duration).count() / count << "ns/op"
              << std::endl;
  }

  {
    auto start = high_resolution_clock::now();
    for (size_t i = 0; i < count; ++i) {
      Function<void()> fun([&state, i]() { state = i; });
      fun();
    }
    auto stop = high_resolution_clock::now();
    auto duration = stop - start;
    std::cout << "Function: "
              << duration_cast<nanoseconds>(duration).count() / count << "ns/op"
              << std::endl;
  }

  return 0;
}
