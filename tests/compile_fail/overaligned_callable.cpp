// SPDX-License-Identifier: MIT
#include <rigtorp/Function.h>

struct alignas(16) OverAligned {
  void operator()() {}
};

rigtorp::Function<void(), 128> invalid{OverAligned{}};
