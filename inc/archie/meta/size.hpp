#pragma once
#include <archie/meta/static_constexpr_storage.hpp>

namespace archie {
namespace meta {
  template <typename...>
  struct type_list;

  namespace detail {
    struct size_ {
      template <typename... T>
      constexpr decltype(auto) operator()(meta::type_list<T...> const&) const noexcept {
        return sizeof...(T);
      }
    };
  }
  constexpr static auto const& size = instance<detail::size_>();
}
}
