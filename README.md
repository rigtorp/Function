# rigtorp::Function

A header-only C++11 alternative to `std::function` that stores callables
inline. Captured objects may still allocate memory.

```cpp
#include <rigtorp/Function.h>

rigtorp::Function<int()> f([] { return 42; });
int result = f();
```

The default capacity is 1024 bytes. Set it with `Function<Signature, MaxSize>`.
Callables must fit and require at most 8-byte alignment. Null function pointers
produce an empty wrapper; calling an empty wrapper throws `std::bad_function_call`.

## Build

Requires CMake 3.20+ and a C++11 compiler.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

## CMake

```cmake
add_subdirectory(path/to/Function)
target_link_libraries(your_target PRIVATE Function::Function)
```

For an installed package, use `find_package(Function CONFIG REQUIRED)`.

## Benchmark

Run `./build/benchmark`. It uses `std::chrono::steady_clock`.

Sample: AMD Ryzen 7 8745HS w/ Radeon 780M Graphics, Linux x86-64,
GCC 16.2.1 (`-O3 -DNDEBUG -std=c++11 -fno-devirtualize`), 2026-10-04.
Medians of five runs, 100 million iterations per case, pinned to logical CPU 0.

| ns/op | std::function | rigtorp::Function | Virtual |
| --- | ---: | ---: | ---: |
| Construction | 9.05 | 0.11 | — |
| Invocation | 1.11 | 1.10 | 1.10 |

Construction includes invocation and destruction. Results reflect compiler
optimizations and depend on the system.
