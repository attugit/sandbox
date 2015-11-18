#include <archie/meta.hpp>
#include <catch.hpp>
#include <vector>
#include <array>

namespace {
struct Iterable {
  template <typename T>
  auto requires(T) -> decltype(std::declval<T>().begin(), std::declval<T>().end());
};

struct HasCapacity {
  template <typename T>
  auto requires(T) -> decltype(std::declval<T const>().capacity());
};

using namespace archie;
TEST_CASE("ignore", "[meta]") {
  SECTION("ctor") {
    meta::ignore_t i0{};
    meta::ignore_t i1{7};
    meta::ignore_t i3{i0, i1, 42};
  }
  SECTION("assigment") {
    ignore = 7;
    ignore = ignore;
  }
}

TEST_CASE("model_of", "[meta]") {
  using vec_t = std::vector<int>;
  using arr_t = std::array<int, 42>;
  vec_t vec;
  arr_t arr;
  REQUIRE(model_of<Iterable>(vec));
  REQUIRE(model_of<Iterable>(arr));
  REQUIRE(model_of<HasCapacity>(vec));
  REQUIRE_FALSE(model_of<HasCapacity>(arr));
  REQUIRE_FALSE(meta::model_of<HasCapacity(arr_t)>::value);
  std::integral_constant<bool, meta::model_of<Iterable(arr_t)>::value &
                                   meta::model_of<HasCapacity(arr_t)>::value> ic;
  REQUIRE_FALSE(ic);
}
}
