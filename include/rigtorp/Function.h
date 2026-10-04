// Copyright (c) 2015 Erik Rigtorp <erik@rigtorp.se>
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>
#include <functional>
#include <new>
#include <type_traits>
#include <utility>

namespace rigtorp {

template <class, std::size_t MaxSize = 1024> class Function;

template <class R, class... Args, std::size_t MaxSize> class Function<R(Args...), MaxSize> {
public:
  Function() noexcept {}

  Function(std::nullptr_t) noexcept {}

  Function(const Function &other) {
    if (other) {
      other.manager(&data, &other.data, Operation::Clone);
      invoker = other.invoker;
      manager = other.manager;
    }
  }

  Function(Function &&other) { moveFrom(other); }

  template <class F,
            typename std::enable_if<!std::is_same<typename std::decay<F>::type,
                                                  Function>::value, int>::type = 0>
  Function(F &&f) {
    using f_type = typename std::decay<F>::type;
    static_assert(alignof(f_type) <= StorageAlignment, "invalid alignment");
    static_assert(sizeof(f_type) <= StorageSize, "storage too small");
    if (isNull(f, std::is_pointer<f_type>{})) {
      return;
    }
    new (&data) f_type(std::forward<F>(f));
    invoker = &invoke<f_type>;
    manager = &manage<f_type>;
  }

  ~Function() {
    if (manager) {
      manager(&data, nullptr, Operation::Destroy);
    }
  }

  Function &operator=(const Function &other) {
    if (this != &other) {
      Function copy(other);
      *this = std::move(copy);
    }
    return *this;
  }

  Function &operator=(Function &&other) {
    if (this != &other) {
      *this = nullptr;
      moveFrom(other);
    }
    return *this;
  }

  Function &operator=(std::nullptr_t) {
    if (manager) {
      manager(&data, nullptr, Operation::Destroy);
      manager = nullptr;
      invoker = nullptr;
    }
    return *this;
  }

  template <typename F,
            typename std::enable_if<!std::is_same<typename std::decay<F>::type,
                                                  Function>::value, int>::type = 0>
  Function &operator=(F &&f) {
    Function(std::forward<F>(f)).swap(*this);
    return *this;
  }

  template <typename F> Function &operator=(std::reference_wrapper<F> f) {
    Function(f).swap(*this);
    return *this;
  }

  void swap(Function &other) {
    if (this != &other) {
      Function temp(std::move(other));
      other = std::move(*this);
      *this = std::move(temp);
    }
  }

  explicit operator bool() const noexcept { return !!manager; }

  R operator()(Args... args) {
    if (!invoker) {
      throw std::bad_function_call();
    }
    return invoker(&data, std::forward<Args>(args)...);
  }

private:
  enum class Operation { Clone, Move, Destroy };

  using Invoker = R (*)(void *, Args &&...);
  using Manager = void (*)(void *, const void *, Operation);
  static constexpr std::size_t StorageAlignment = 8;
  static constexpr std::size_t Overhead = sizeof(Invoker) + sizeof(Manager);
  static_assert(MaxSize > Overhead, "MaxSize must leave room for a callable after the function pointers");
  // Guard the subtraction and array bound even when the assertion fails.
  static constexpr std::size_t StorageSize = MaxSize > Overhead ? MaxSize - Overhead : 1;

  template <typename F> static bool isNull(const F &f, std::true_type) noexcept {
    return f == nullptr;
  }

  template <typename F> static bool isNull(const F &, std::false_type) noexcept {
    return false;
  }

  template <typename F>
  static R invoke(void *data, Args &&... args) {
    F &f = *static_cast<F *>(data);
    return f(std::forward<Args>(args)...);
  }

  template <typename F>
  static void manage(void *dest, const void *src, Operation op) {
    switch (op) {
    case Operation::Clone:
      new (dest) F(*static_cast<const F *>(src));
      break;
    case Operation::Move:
      // Move is only requested for a non-const source. Copy-only callables
      // and callables with throwing moves can use their copy constructor.
      new (dest) F(std::move_if_noexcept(*static_cast<F *>(const_cast<void *>(src))));
      break;
    case Operation::Destroy:
      static_cast<F *>(dest)->~F();
      break;
    }
  }

  void moveFrom(Function &other) {
    if (other) {
      other.manager(&data, &other.data, Operation::Move);
      invoker = other.invoker;
      manager = other.manager;
      other = nullptr;
    }
  }

  alignas(StorageAlignment) unsigned char data[StorageSize];
  Invoker invoker = nullptr;
  Manager manager = nullptr;
};

} // namespace rigtorp
