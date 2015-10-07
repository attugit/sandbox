#pragma once
#include <utility>
#include <type_traits>

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
      typename std::enable_if<!std::is_convertible<U, pointer>::value && std::is_empty<U>::value,
                              void*>::type = nullptr)
      : pure_function([](Args... xs) { return (*static_cast<U*>(nullptr))(xs...); }) {}

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
  typename std::enable_if<!std::is_convertible<U, pointer>::value && std::is_empty<U>::value,
                          pure_function&>::type
  operator=(U u) {
    fptr = pure_function{u};
    return *this;
  }

  explicit operator bool() const { return fptr != nullptr; }

  operator pointer() const { return fptr; }

private:
  pointer fptr = nullptr;
};
}
