#pragma once
#include <archie/meta/static_constexpr_storage.hpp>

namespace archie {
namespace meta {
  struct ignore_t final {
    template <typename... Args>
    constexpr ignore_t(Args&&...) noexcept {}
    template <typename T>
    constexpr ignore_t const& operator=(T const&) const noexcept {
      return *this;
    }
  };
}

static constexpr auto const& ignore = meta::instance<meta::ignore_t>();
}
