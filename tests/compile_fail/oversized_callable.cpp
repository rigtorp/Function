// SPDX-License-Identifier: MIT
#include <rigtorp/Function.h>

constexpr std::size_t overhead = sizeof(void (*)(void *)) +
                                 sizeof(void (*)(void *, const void *, int));
struct TooLarge {
  unsigned char bytes[24];
  void operator()() {}
};

// Alignment padding must not enlarge the advertised callable capacity.
rigtorp::Function<void(), overhead + 17> invalid{TooLarge{}};
