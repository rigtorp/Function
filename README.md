# Function.h

Heap allocation free version of C++11 *std::function*.

Function.h stores the closure in an internal buffer instead of heap
allocated memory. This is useful for low latency agent and thread pool
systems. Please note that the captured values can perfom allocations,
for example *std::string*.

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
