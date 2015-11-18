#pragma once
#include <archie/meta/static_constexpr_storage.hpp>

namespace archie {
namespace meta {
  template <typename...>
  struct type_list;

  namespace detail {
    struct tail_ {
      template <typename T, typename... U>
      constexpr decltype(auto) operator()(meta::type_list<T, U...> const&) const noexcept {
        return instance<meta::type_list<U...>>();
      }
      constexpr decltype(auto) operator()(meta::type_list<> const&) const noexcept {
        return instance<meta::type_list<>>();
      }
    };
  }
  constexpr static auto const& tail = instance<detail::tail_>();
}
}
