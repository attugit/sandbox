#pragma once
#include <archie/meta/ignore.hpp>

namespace archie {
namespace meta {
#if __GNUC__ < 5
  template <typename...>
  struct well_formed_ {
    using type = ignore_t;
  };
  template <typename... T>
  using well_formed = typename well_formed_<T...>::type;
#else
  template <typename...>
  using well_formed = ignore_t;
#endif
}
}
