// SPDX-License-Identifier: MIT

#include <rigtorp/Function.h>
#include <chrono>
#include <iomanip>
#include <iostream>

int main() {
  constexpr size_t count = 100000000;

  using namespace std::chrono;
  using Nanoseconds = std::chrono::duration<double, std::nano>;
  std::cout << std::fixed << std::setprecision(2);

  volatile size_t state = 0;
  struct State {
    size_t a, b, c;
  };
  State state2 = {0, 0, 0};

  std::cout << "construction overhead" << std::endl;

  {
    auto start = steady_clock::now();
    for (size_t i = 0; i < count; ++i) {
      std::function<void()> stdfun = [&state, state2, i]() { state = i; };
      stdfun();
    }
    auto stop = steady_clock::now();
    auto duration = stop - start;
    std::cout << "std::function: "
              << duration_cast<Nanoseconds>(duration).count() / count << "ns/op"
              << std::endl;
  }

  {
    auto start = steady_clock::now();
    for (size_t i = 0; i < count; ++i) {
      rigtorp::Function<void()> fun = [&state, state2, i]() { state = i; };
      fun();
    }
    auto stop = steady_clock::now();
    auto duration = stop - start;
    std::cout << "Function: "
              << duration_cast<Nanoseconds>(duration).count() / count << "ns/op"
              << std::endl;
  }

  std::cout << "invocation overhead" << std::endl;

  {
    std::function<void(size_t)> stdfun([&state](size_t i) { state = i; });
    auto start = steady_clock::now();
    for (size_t i = 0; i < count; ++i) {
      stdfun(i);
    }
    auto stop = steady_clock::now();
    auto duration = stop - start;
    std::cout << "std::function: "
              << duration_cast<Nanoseconds>(duration).count() / count << "ns/op"
              << std::endl;
  }

  {
    rigtorp::Function<void(size_t)> fun([&state](size_t i) { state = i; });
    auto start = steady_clock::now();
    for (size_t i = 0; i < count; ++i) {
      fun(i);
    }
    auto stop = steady_clock::now();
    auto duration = stop - start;
    std::cout << "Function: "
              << duration_cast<Nanoseconds>(duration).count() / count << "ns/op"
              << std::endl;
  }

  {
    // Must compile this with -fno-devirtualize to prevent inlining
    struct Fun {
      virtual void fun(size_t) = 0;
    };
    struct Impl : public Fun {
      void fun(size_t i) override { state = i; };
      volatile size_t state = 0;
    };
    Impl impl;
    auto start = steady_clock::now();
    for (size_t i = 0; i < count; ++i) {
      Fun *fun = &impl;
      fun->fun(i);
    }
    auto stop = steady_clock::now();
    auto duration = stop - start;
    std::cout << "virtual: "
              << duration_cast<Nanoseconds>(duration).count() / count << "ns/op"
              << std::endl;
  }

  return 0;
}
