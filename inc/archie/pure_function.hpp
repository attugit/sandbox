#pragma once
#include <utility>
#include <type_traits>
#include <archie/static_constexpr_storage.hpp>

namespace archie {
template <typename...>
struct pure_function;

template <typename R, typename... Args>
struct pure_function<R(Args...)> {
  using type = R (*)(Args...);
  using pointer = type;

  pure_function() = default;

  template <typename U>
  explicit pure_function(
      U u,
      typename std::enable_if<std::is_convertible<U, pointer>::value, void*>::type = nullptr)
      : fptr(u) {}

  template <typename U>
  explicit pure_function(
      U,
      typename std::enable_if<!std::is_convertible<U, pointer>::value && std::is_empty<U>::value &&
                                  std::is_trivially_constructible<U>::value,
                              void*>::type = nullptr)
      : pure_function([](Args... xs) { return std::add_const_t<U>{}(std::forward<Args>(xs)...); }) {
  }

  template <typename... U>
  auto operator()(U&&... u) const {
    return (*fptr)(std::forward<U>(u)...);
  }

  template <typename U>
  typename std::enable_if<std::is_convertible<U, pointer>::value, pure_function&>::type operator=(
      U u) {
    fptr = u;
    return *this;
  }

  template <typename U>
  typename std::enable_if<!std::is_convertible<U, pointer>::value && std::is_empty<U>::value &&
                              std::is_trivially_constructible<U>::value,
                          pure_function&>::type
  operator=(U) {
    fptr = [](Args... xs) { return std::add_const_t<U>{}(std::forward<Args>(xs)...); };
    return *this;
  }

  explicit operator bool() const { return fptr != nullptr; }

  operator pointer() const { return fptr; }

private:
  pointer fptr = nullptr;
};
}
