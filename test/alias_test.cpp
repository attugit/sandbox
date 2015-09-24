#include <archie/alias.hpp>
#include <vector>
#include <array>
#include <catch.hpp>
namespace {
using namespace archie;

TEST_CASE("alias", "[alias]") {
  SECTION("ref") {
    int x = 0;
    int y = 1;
    auto a = alias(x);
    REQUIRE(*a == 0);
    a = 2;
    REQUIRE(x == 2);
    a = rebind(y);
    a = 3;
    REQUIRE(x == 2);
    REQUIRE(y == 3);
  }
  SECTION("const ref") {
    const int x = 0;
    const int y = 1;
    auto a = alias(x);
    auto b = alias(x);
    REQUIRE(*a == 0);
    a = rebind(y);
    REQUIRE(*a == 1);
    a = rebind(b);
    REQUIRE(*a == 0);
  }
  SECTION("sorting") {
    std::array<int, 3> array = {{3, 1, 2}};
    std::vector<alias_t<int>> vec;
    vec.emplace_back(array[1]);
    vec.emplace_back(array[2]);
    vec.emplace_back(array[0]);
    std::sort(std::begin(vec), std::end(vec));
    REQUIRE(*(vec[0]) == 3);
    REQUIRE(*(vec[1]) == 1);
    REQUIRE(*(vec[2]) == 2);
    REQUIRE(array[0] == 3);
    REQUIRE(array[1] == 1);
    REQUIRE(array[2] == 2);
    std::sort(std::begin(vec), std::end(vec), [](auto const& lhs, auto const& rhs) {
      return *lhs < *rhs;
    });
    REQUIRE(*(vec[0]) == 1);
    REQUIRE(*(vec[1]) == 2);
    REQUIRE(*(vec[2]) == 3);
    REQUIRE(array[0] == 3);
    REQUIRE(array[1] == 1);
    REQUIRE(array[2] == 2);
  }
}
}
