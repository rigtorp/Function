// SPDX-License-Identifier: MIT

#include <rigtorp/Function.h>
#include <cstdlib>
#include <stdexcept>
#include <string>

void check(bool condition) {
  if (!condition) std::abort();
}

struct Callable {
  static int alive;
  const Callable *self;
  std::string value;
  Callable() : self(this), value(100, 'x') { ++alive; }
  Callable(const Callable &other) : self(this), value(other.value) { ++alive; }
  Callable(Callable &&other) noexcept
      : self(this), value(std::move(other.value)) { ++alive; }
  ~Callable() { check(self == this); --alive; }
  bool operator()() const { return self == this && value == std::string(100, 'x'); }
};
int Callable::alive = 0;

struct CopyOnly {
  CopyOnly() = default;
  CopyOnly(const CopyOnly &) = default;
  CopyOnly(CopyOnly &&) = delete;
  int operator()() const { return 7; }
};

struct ThrowingCopy {
  static bool fail;
  ThrowingCopy() = default;
  ThrowingCopy(const ThrowingCopy &) {
    if (fail) throw std::runtime_error("copy");
  }
  int operator()() const { return 8; }
};
bool ThrowingCopy::fail = false;

struct CheckedCopy {
  static int alive;
  static int copiesBeforeThrow;
  const CheckedCopy *self;
  CheckedCopy() : self(this) { ++alive; }
  CheckedCopy(const CheckedCopy &) : self(this) {
    if (copiesBeforeThrow == 0) throw std::runtime_error("copy");
    if (copiesBeforeThrow > 0) --copiesBeforeThrow;
    ++alive;
  }
  ~CheckedCopy() { check(self == this); --alive; }
  bool operator()() const { return self == this; }
};
int CheckedCopy::alive = 0;
int CheckedCopy::copiesBeforeThrow = -1;

int functionTarget() { return 42; }
int noexceptTarget() noexcept { return 43; }

template <typename Pointer> void checkPointerTargets(Pointer pointer, int expected) {
  rigtorp::Function<int()> function(pointer);
  check(function() == expected);
  Pointer null = nullptr;
  rigtorp::Function<int()> empty(null);
  check(!empty);
  const rigtorp::Function<int()> &ref = empty;
  rigtorp::Function<int()> copy(ref);
  rigtorp::Function<int()> moved(std::move(empty));
  check(!copy && !moved && !empty);
  try {
    moved();
    std::abort();
  } catch (const std::bad_function_call &) {}
  function = null;
  check(!function);
  function = pointer;
  check(function() == expected);
}

int main() {
  checkPointerTargets(&functionTarget, 42);
  checkPointerTargets(&noexceptTarget, 43);
  static_assert(sizeof(rigtorp::Function<void()>) == 1024, "default wrapper size changed");
  static_assert(sizeof(rigtorp::Function<void(), 128>) == 128, "wrapper size changed");
  static_assert(alignof(rigtorp::Function<void()>) == 8, "wrapper alignment changed");
  // A one-byte callable fits even at the minimum valid capacity.
  rigtorp::Function<int(), 2 * sizeof(void (*)()) + 1> smallest([] { return 5; });
  check(smallest() == 5);
  {
    rigtorp::Function<bool()> original{Callable{}};
    const rigtorp::Function<bool()> &ref = original;
    rigtorp::Function<bool()> mutableCopy(original);
    rigtorp::Function<bool()> constCopy(ref);
    check(original() && mutableCopy() && constCopy());
    rigtorp::Function<bool()> moved(std::move(mutableCopy));
    check(moved() && !mutableCopy);
    rigtorp::Function<bool()> assigned;
    assigned = ref;
    check(assigned());
    assigned = original;
    check(assigned() && original());
    assigned = std::move(moved);
    check(assigned() && !moved);
    original.swap(assigned);
    check(original() && assigned());
    rigtorp::Function<bool()> empty;
    empty.swap(original);
    check(empty() && !original);
    original.swap(empty);
    check(original() && !empty);
    auto *same = &original;
    original = *same;
    original = std::move(*same);
    original.swap(*same);
    check(original());
    assigned = Callable{};
    check(assigned());
    assigned = nullptr;
    check(!assigned);
    const rigtorp::Function<bool()> &emptyRef = empty;
    rigtorp::Function<bool()> emptyCopy(emptyRef);
    rigtorp::Function<bool()> emptyMove(std::move(empty));
    check(!emptyCopy && !emptyMove);
    original = emptyRef;
    check(!original);
    try {
      original();
      std::abort();
    } catch (const std::bad_function_call &) {}
  }
  check(Callable::alive == 0);
  rigtorp::Function<int()> first([] { return 1; });
  rigtorp::Function<int()> second([] { return 2; });
  first.swap(second);
  check(first() == 2 && second() == 1);
  first = second;
  check(first() == 1 && second() == 1);
  first = std::move(second);
  check(first() == 1 && !second);
  int calls = 0;
  auto callback = [&calls] { return ++calls; };
  rigtorp::Function<int()> reference(std::ref(callback));
  first = std::ref(callback);
  check(reference() == 1 && first() == 2 && calls == 2);
  CopyOnly callable;
  rigtorp::Function<int()> copyOnly(callable);
  rigtorp::Function<int()> copyOnlyMoved(std::move(copyOnly));
  check(copyOnlyMoved() == 7 && !copyOnly);
  rigtorp::Function<int()> throwing{ThrowingCopy{}};
  rigtorp::Function<int()> destination([] { return 9; });
  ThrowingCopy::fail = true;
  auto *sameThrowing = &throwing;
  throwing = *sameThrowing;
  throwing = std::move(*sameThrowing);
  throwing.swap(*sameThrowing);
  check(throwing() == 8);
  try {
    destination = static_cast<const rigtorp::Function<int()> &>(throwing);
    std::abort();
  } catch (const std::runtime_error &) {}
  check(destination() == 9 && throwing() == 8);
  try {
    rigtorp::Function<int()> moved(std::move(throwing));
    std::abort();
  } catch (const std::runtime_error &) {}
  check(throwing() == 8);
  try {
    destination = std::move(throwing);
    std::abort();
  } catch (const std::runtime_error &) {}
  check(!destination && throwing() == 8);
  // Every relocation stage in swap must leave only live objects behind,
  // even if a copy-only callable throws partway through the exchange.
  for (int stage = 0; stage != 3; ++stage) {
    CheckedCopy::copiesBeforeThrow = -1;
    {
      CheckedCopy callable;
      rigtorp::Function<bool()> left(callable), right(callable);
      CheckedCopy::copiesBeforeThrow = stage;
      try {
        left.swap(right);
        std::abort();
      } catch (const std::runtime_error &) {}
      if (left) check(left());
      if (right) check(right());
    }
    check(CheckedCopy::alive == 0);
  }
}
