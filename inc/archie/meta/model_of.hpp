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

template <typename Concept, typename... Args>
constexpr bool model_of(Args...) {
  return meta::model_of<Concept(Args...)>::value;
}
}
