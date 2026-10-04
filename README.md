# Function.h

Heap allocation free version of C++11 *std::function*.

Function.h stores the closure in an internal buffer instead of heap
allocated memory. This is useful for low latency agent and thread pool
systems. Please note that the captured values can perform allocations,
for example *std::string*.

Copying invokes the stored callable's copy constructor. Moving and swapping
invoke its move constructor when it is non-throwing, otherwise its copy
constructor, and destroy the source object after successful transfer. A
successfully moved-from `Function` is empty. Copy-only callables are supported.
Callable destructors must not throw.

If a callable constructor throws during move assignment or swap, the affected
`Function` objects remain valid but may be empty. Copy assignment preserves
the destination if the initial copy fails; a failure during the subsequent
transfer can leave the destination empty.

Build the example, benchmark, and regression tests with CMake 3.20 or newer
and a C++11 compiler:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

`FUNCTION_BUILD_EXAMPLES`, `FUNCTION_BUILD_BENCHMARKS`, and
`FUNCTION_BUILD_TESTS` control the optional executables. `BUILD_TESTING=OFF`
also disables the tests. These executables and installation rules default
to off when Function is included in another project.

To use a checkout in another CMake project:

```cmake
add_subdirectory(path/to/Function)
target_link_libraries(your_target PRIVATE Function::Function)
```

Or install the header and CMake package to a prefix:

```sh
cmake --install build --prefix /path/to/prefix
```

Configure a consumer with `-DCMAKE_PREFIX_PATH=/path/to/prefix` and use:

```cmake
find_package(Function CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE Function::Function)
```

`FUNCTION_INSTALL=OFF` disables installation rules. The library requires
C++11; it does not impose benchmark compiler flags on consumers.

## Benchmark

The dependency-free benchmark uses `std::chrono::steady_clock` and reports
fractional nanoseconds per operation. Build and run it in Release mode:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DFUNCTION_BUILD_BENCHMARKS=ON
cmake --build build --target benchmark
./build/benchmark
```

Sample results measured on 2026-10-04 on an **AMD Ryzen 7 8745HS w/ Radeon
780M Graphics**, using **GCC 16.2.1** on Linux x86-64, compiled with
`-O3 -DNDEBUG -std=c++11 -fno-devirtualize`. Values below are the median of
five runs, each with 100,000,000 iterations per case, pinned to logical CPU 0
using `taskset -c 0 ./build/benchmark`:

```
construction overhead
  std::function: 9.05ns/op
  Function:     0.11ns/op
  
invocation overhead:
  std::function: 1.11ns/op
  Function:     1.10ns/op
  virtual:      1.10ns/op
```

The construction case includes construction, invocation, and destruction.
The compiler can simplify these loops, especially the inline-storage case;
these measurements describe this benchmark rather than isolated constructor
costs. Results depend on the compiler, CPU, and system load.

## License

MIT; see [LICENSE](LICENSE). Source files use SPDX license identifiers.
