#pragma once
#include <archie/meta/ignore.hpp>

namespace archie {
namespace meta {
  template <typename C>
  using requires = typename std::enable_if<C::value, ignore_t>::type;
}
}
