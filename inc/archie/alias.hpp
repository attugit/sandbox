#pragma once
#include <memory>
#include <archie/static_constexpr_storage.hpp>

namespace archie {
template <typename>
struct alias_t;

template <typename T>
using rebind_t = typename alias_t<T>::rebind_t;

namespace detail {
  struct alias_ {
    template <typename T>
    constexpr decltype(auto) operator()(T& t) const noexcept {
      return alias_t<T>(t);
    }
  };

  struct rebind_ {
    template <typename T>
    constexpr decltype(auto) operator()(T& t) const noexcept {
      return rebind_t<T>(t);
    }
    template <typename T>
    constexpr decltype(auto) operator()(alias_t<T> a) const noexcept {
      return rebind_t<T>(*a);
    }
  };

  struct unwrap_ {
    template <typename T>
    constexpr decltype(auto) operator()(alias_t<T> a) const noexcept {
      return *a;
    }
  };
}

static constexpr auto const& alias = meta::instance<detail::alias_>();
static constexpr auto const& rebind = meta::instance<detail::rebind_>();
static constexpr auto const& unwrap = meta::instance<detail::unwrap_>();

template <typename T>
struct alias_t {
  using value_type = T;
  using pointer = value_type*;
  using const_pointer = value_type const*;
  using reference = value_type&;
  using const_reference = value_type const&;

  struct rebind_t {
    friend alias_t;
    rebind_t(reference x) noexcept : data_(std::addressof(x)) {}

  private:
    pointer data_;
  };

  alias_t() = delete;
  alias_t(value_type&&) = delete;
  alias_t& operator=(alias_t const&) = delete;
  alias_t(alias_t const&) noexcept = default;
  alias_t(alias_t&&) noexcept = default;
  alias_t& operator=(alias_t&&) noexcept = default;

  explicit alias_t(rebind_t r) noexcept : ref_(r) {}
  explicit alias_t(reference indata) noexcept : ref_(indata) {}

  template <typename U>
  alias_t(alias_t<U> const& a) noexcept : ref_(unwrap(a)) {}

  alias_t& operator=(rebind_t r) noexcept {
    ref_ = r;
    return *this;
  }

  alias_t& operator=(const_reference x) noexcept {
    if (std::addressof(unwrap(*this)) != std::addressof(x)) unwrap(*this) = x;
    return *this;
  }

  alias_t& operator=(value_type&& x) noexcept {
    if (std::addressof(unwrap(*this)) != std::addressof(x)) unwrap(*this) = std::move(x);
    return *this;
  }

  reference operator*() noexcept { return *ref_.data_; }
  const_reference operator*() const noexcept { return *ref_.data_; }

  pointer operator->() noexcept { return ref_.data_; }
  const_pointer operator->() const noexcept { return ref_.data_; }

private:
  rebind_t ref_;
};

template <typename T>
bool operator==(alias_t<T> const& lhs, alias_t<T> const& rhs) noexcept {
  return std::addressof(unwrap(lhs)) == std::addressof(unwrap(rhs));
}

template <typename T>
bool operator!=(alias_t<T> const& lhs, alias_t<T> const& rhs) noexcept {
  return !(lhs == rhs);
}

template <typename T>
bool operator<(alias_t<T> const& lhs, alias_t<T> const& rhs) noexcept {
  return std::addressof(unwrap(lhs)) < std::addressof(unwrap(rhs));
}

template <typename T>
bool operator>(alias_t<T> const& lhs, alias_t<T> const& rhs) noexcept {
  return rhs < lhs;
}

template <typename T>
bool operator<=(alias_t<T> const& lhs, alias_t<T> const& rhs) noexcept {
  return !(lhs > rhs);
}

template <typename T>
bool operator>=(alias_t<T> const& lhs, alias_t<T> const& rhs) noexcept {
  return !(lhs < rhs);
}
}
