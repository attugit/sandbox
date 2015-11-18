#pragma once
#include <utility>
#include <type_traits>
#include <archie/meta/static_constexpr_storage.hpp>
#include <archie/meta/requires.hpp>

namespace archie {
namespace detail {
  template <typename...>
  struct to_function_pointer_;

  template <typename R, typename... Args>
  struct to_function_pointer_<R(Args...)> {
    using pointer = R (*)(Args...);

  private:
#if __GNUC__ < 5
    template <typename T>
    using trivial = std::has_trivial_default_constructor<T>;
#else
    template <typename T>
    using trivial = std::is_trivially_default_constructible<T>;
#endif
    template <typename T>
    struct is_stateless : std::integral_constant<bool,
                                                 !std::is_convertible<T, pointer>::value &&
                                                     std::is_empty<T>::value &&
                                                     trivial<T>::value> {};

  public:
    template <typename T, meta::requires<std::is_convertible<T, pointer>>...>
    pointer operator()(T t) const {
      return t;
    }

    template <typename T, meta::requires<is_stateless<T>>...>
    pointer operator()(T) const {
      return this->operator()([](Args... args) {
        return typename std::add_const<T>::type{}(std::forward<Args>(args)...);
      });
    }
  };
}

template <typename...>
struct pure_function;

template <typename R, typename... Args>
struct pure_function<R(Args...)> {
  using type = R (*)(Args...);
  using pointer = type;

  pure_function() = default;

  template <typename T>
  explicit pure_function(T t)
      : fptr(meta::instance<detail::to_function_pointer_<R(Args...)>>()(t)) {}

  template <typename T>
  pure_function& operator=(T t) {
    fptr = meta::instance<detail::to_function_pointer_<R(Args...)>>()(t);
    return *this;
  }

  template <typename... U>
  auto operator()(U&&... u) const {
    return (*fptr)(std::forward<U>(u)...);
  }

  explicit operator bool() const { return fptr != nullptr; }
  operator pointer() const { return fptr; }

private:
  pointer fptr = nullptr;
};
}
