#pragma once
#include <utility>

namespace archie {

struct null_inapt_t {};

template <typename T, typename P>
struct inapt {
  using value_type = T;
  using type = value_type;
  using pointer = T*;
  using const_pointer = T const*;
  using reference = T&;
  using const_reference = T const&;

private:
  struct impl_ : P {
    impl_() : P(), value(P::null()) {}
    bool is_null() const { return P::is_null(value); }
    template <typename... U>
    impl_(U&&... u)
        : value(std::forward<U>(u)...) {}
    void set_null() { value = P::null(); }
    value_type value;
  };
  impl_ impl;

public:
  inapt() = default;
  inapt(inapt const&) = default;
  inapt(inapt&&) = default;
  inapt& operator=(inapt const&) = default;
  inapt& operator=(inapt&&) = default;

  template <typename... U>
  explicit inapt(U&&... u)
      : impl(std::forward<U>(u)...) {}

  template <typename U>
  inapt& operator=(U const& u) {
    impl.value = u;
    return *this;
  }

  inapt& operator=(null_inapt_t const&) {
    impl.set_null();
    return *this;
  }

  explicit operator bool() const { return !is_null(); }
  explicit operator const_reference() const { return get(); }

  bool is_null() const { return impl.is_null(); }
  const_reference get() const { return impl.value; }
  reference get() { return impl.value; }

  bool operator==(null_inapt_t const&) const { return is_null(); }
  bool operator!=(null_inapt_t const&) const { return !is_null(); }
  template <typename U>
  bool operator==(U const& rhs) const {
    return get() == rhs;
  }
  template <typename U>
  bool operator!=(U const& rhs) const {
    return !(get() == rhs);
  }

  reference operator*() { return get(); }
  const_reference operator*() const { return get(); }
  pointer operator->() { return &get(); }
  const_pointer operator->() const { return &get(); }
};

template <typename T, T...>
struct reserved_value;

template <typename T, T v0, T... vs>
struct reserved_value<T, v0, vs...> {
  T null() const { return v0; }
  bool is_null(T const& x) const { return (x == v0) || reserved_value<T, vs...>{}.is_null(x); }
};

template <typename T, T v0>
struct reserved_value<T, v0> {
  T null() const { return v0; }
  bool is_null(T const& x) const { return x == v0; }
};

template <typename T, T... v>
using reserved = inapt<T, reserved_value<T, v...>>;

template <typename T>
using reserved_default = reserved<T, T{}>;
}
