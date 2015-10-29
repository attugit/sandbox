#pragma once
#include <archie/meta/ignore.hpp>

namespace archie {
namespace meta {
  template <typename...>
  using well_formed = ignore_t;
}
}
