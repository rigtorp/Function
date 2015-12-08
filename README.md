# Function.h

Heap allocation free version of C++11 *std::function*.

Function.h stores the closure in an internal buffer instead of heap
allocated memory. This is useful for low latency agent and thread pool
systems. Please note that the captured values can perfom allocations,
for example *std::string*.

## Benchmark

*Function.h* is almost 3 times faster than *std::function*.

```
std::function: 87ns/op
Function:      29ns/op
```
