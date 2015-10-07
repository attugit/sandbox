#pragma once
#include <vector>
#include <utility>
#include <type_traits>

namespace archie {
template <typename...>
struct function_chain;

template <typename R, typename... Args>
struct function_chain<R(Args...)> : std::vector<R (*)(Args...)> {
private:
  using fptr_t = R (*)(Args...);
  using base_t = std::vector<fptr_t>;

public:
  using base_t::base_t;
  template <typename F>
  void push_back(F f) {
    static_assert(std::is_empty<F>::value == true, "");
    base_t::push_back(f);
  }
};

template <typename...>
struct pure_function;

template <typename R, typename... Args>
struct pure_function<R(Args...)> {
  using type = R (*)(Args...);
  using pointer = type;

  pure_function() = default;

  template <typename U,
            typename std::enable_if<std::is_convertible<U, pointer>::value, int>::type...>
  explicit pure_function(U u)
      : fptr(u) {}

  template <
      typename U,
      typename std::enable_if<!std::is_convertible<U, pointer>::value && std::is_empty<U>::value,
                              int>::type...>
  explicit pure_function(U)
      : pure_function([](Args... xs) { return (*static_cast<U*>(nullptr))(xs...); }) {}

  template <typename... U>
  auto operator()(U&&... u) const {
    return (*fptr)(std::forward<U>(u)...);
  }

  template <typename U,
            typename std::enable_if<std::is_convertible<U, pointer>::value, int>::type...>
  pure_function& operator=(U u) {
    fptr = u;
    return *this;
  }

  template <
      typename U,
      typename std::enable_if<!std::is_convertible<U, pointer>::value && std::is_empty<U>::value,
                              int>::type...>
  pure_function& operator=(U) {
    fptr = [](Args... xs) { return (*static_cast<U*>(nullptr))(xs...); };
    return *this;
  }

  explicit operator bool() const { return fptr != nullptr; }

  operator pointer() const { return fptr; }

private:
  pointer fptr = nullptr;
};
}
