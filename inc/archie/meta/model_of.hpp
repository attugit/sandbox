#pragma once
#include <utility>
#include <archie/meta/well_formed.hpp>

namespace archie {
namespace meta {
  template <typename, typename = well_formed<>>
  struct model_of : std::false_type {};

  template <typename Concept, typename... Args>
  struct model_of<Concept(Args...),
                  well_formed<decltype(std::declval<Concept>().requires(std::declval<Args>()...))>>
      : std::true_type {};
}
namespace detail {
  template <typename Concept>
  struct model_of_ {
    template <typename... Args>
    constexpr decltype(auto) operator()(Args...) const noexcept {
      return meta::model_of<Concept(Args...)>::value;
    }
  };
}

template <typename Concept>
static constexpr auto const& model_of = meta::instance<detail::model_of_<Concept>>();
}
