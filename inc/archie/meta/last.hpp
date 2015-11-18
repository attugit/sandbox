#pragma once
#include <archie/meta/static_constexpr_storage.hpp>
#include <archie/meta/ignore.hpp>
#include <archie/meta/indexable.hpp>

namespace archie {
namespace meta {
  template <typename...>
  struct type_list;

  namespace detail {
    struct last_ {
    private:
      template <std::size_t>
      using eat = meta::ignore_t;

      template <std::size_t... n>
      struct impl_t {
        template <typename T, typename... U>
        constexpr decltype(auto) operator()(eat<n>..., T, U...) const noexcept {}
      };

    public:
      template <typename T, typename... U>
      constexpr decltype(auto) operator()(meta::type_list<T, U...> const&) const noexcept {
        return instance<meta::type_list<T>>();
      }
      constexpr decltype(auto) operator()(meta::type_list<> const&) const noexcept {
        return instance<meta::type_list<>>();
      }
    };
  }
  constexpr static auto const& last = instance<detail::last_>();
}
}
