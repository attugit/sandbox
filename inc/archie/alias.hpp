#pragma once
#include <memory>
#include <archie/static_constexpr_storage.hpp>

namespace archie {
template <typename T>
struct alias_t {
  using value_type = T;
  using type = value_type;
  using pointer = value_type*;
  using const_pointer = value_type const*;
  using reference = value_type&;
  using const_reference = value_type const&;

  struct rebind_t {
    rebind_t(reference x) : data_(std::addressof(x)) {}
    rebind_t(rebind_t const& x) = default;
    reference get() { return *data_; }
    const_reference get() const { return *data_; }

  private:
    pointer data_;
  };

  alias_t() = delete;
  explicit alias_t(rebind_t r) : ref_(r) {}
  explicit alias_t(reference indata) noexcept : ref_(indata) {}

  alias_t(value_type&&) = delete;
  alias_t(alias_t const&) = default;
  alias_t(alias_t&&) = default;
  template <typename U>
  alias_t(alias_t<U> const& a)
      : ref_(a.get()) {}

  alias_t& operator=(alias_t const&) = delete;
  alias_t& operator=(alias_t&&) = default;

  alias_t& operator=(rebind_t r) {
    ref_ = r;
    return *this;
  }
  alias_t& operator=(const_reference x) {
    if (&get() != &x) { get() = x; }
    return *this;
  }
  alias_t& operator=(value_type&& x) {
    if (&get() != &x) { get() = std::move(x); }
    return *this;
  }

  reference get() { return ref_.get(); }
  const_reference get() const { return ref_.get(); }

  reference operator*() { return get(); }
  const_reference operator*() const { return get(); }

  pointer operator->() { return &get(); }
  const_pointer operator->() const { return &get(); }

private:
  rebind_t ref_;
};

template <typename T>
bool operator==(alias_t<T> const& lhs, alias_t<T> const& rhs) {
  return &lhs.get() == &rhs.get();
}

template <typename T>
bool operator!=(alias_t<T> const& lhs, alias_t<T> const& rhs) {
  return !(lhs == rhs);
}

template <typename T>
bool operator<(alias_t<T> const& lhs, alias_t<T> const& rhs) {
  return &lhs.get() < &rhs.get();
}

template <typename T>
bool operator>(alias_t<T> const& lhs, alias_t<T> const& rhs) {
  return rhs < lhs;
}

template <typename T>
bool operator<=(alias_t<T> const& lhs, alias_t<T> const& rhs) {
  return !(lhs > rhs);
}

template <typename T>
bool operator>=(alias_t<T> const& lhs, alias_t<T> const& rhs) {
  return !(lhs < rhs);
}

template <typename T>
using rebind_t = typename alias_t<T>::rebind_t;

namespace detail {
  struct alias_ {
    template <typename T>
    constexpr alias_t<T> operator()(T& t) const noexcept {
      return alias_t<T>(t);
    }
  };

  struct rebind_ {
    template <typename T>
    constexpr rebind_t<T> operator()(T& t) const noexcept {
      return rebind_t<T>(t);
    }
    template <typename T>
    constexpr rebind_t<T> operator()(alias_t<T> a) const noexcept {
      return rebind_t<T>(*a);
    }
  };

  struct unwrap_ {
    template <typename T>
    constexpr auto operator()(alias_t<T> a) const noexcept {
      return *a;
    }
  };
}

static constexpr auto const& alias = meta::instance<detail::alias_>();
static constexpr auto const& rebind = meta::instance<detail::rebind_>();
static constexpr auto const& unwrap = meta::instance<detail::unwrap_>();
}
