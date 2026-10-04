// SPDX-License-Identifier: MIT
#include <rigtorp/Function.h>

constexpr std::size_t overhead = sizeof(void (*)(void *)) +
                                 sizeof(void (*)(void *, const void *, int));
rigtorp::Function<void(), overhead - 1> invalid;
