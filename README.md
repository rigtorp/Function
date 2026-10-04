# Function.h

Heap allocation free version of C++11 *std::function*.

Function.h stores the closure in an internal buffer instead of heap
allocated memory. This is useful for low latency agent and thread pool
systems. Please note that the captured values can perfom allocations,
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

Build the examples and run the regression tests with:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Benchmark

*Function.h* is quite a lot faster than *std::function* to
 construct. Invocation overhead is the same for both.

Compiled with `gcc -O3 -fno-devirtualize` (gcc 5.2).

```
construction overhead
  std::function: 42ns/op
  Function:      4ns/op
  
invokation overhead:
  std::function: 2ns/op
  Function:      2ns/op
  virtual:       2ns/op
```
