#include <archie/meta/type_list.hpp>
#include <archie/meta/size.hpp>
#include <archie/meta/head.hpp>
#include <archie/meta/tail.hpp>
#include <archie/meta/last.hpp>
#include <catch.hpp>

namespace {
using namespace archie::meta;
TEST_CASE("algorithms", "[type_list]") {
  constexpr auto const& l0 = instance<type_list<>>();
  constexpr auto const& l1 = instance<type_list<char>>();
  constexpr auto const& l2 = instance<type_list<int, char>>();
  constexpr auto const& l3 = instance<type_list<void, int, char>>();
  constexpr auto const& lvoid = instance<type_list<void>>();
  constexpr auto const& lint = instance<type_list<int>>();
  constexpr auto const& lchar = instance<type_list<char>>();
  SECTION("compare") {
    REQUIRE(l0 == l0);
    REQUIRE_FALSE(l0 == l1);
    REQUIRE(l2 != l3);
    REQUIRE_FALSE(l2 != l2);
  }
  SECTION("size") {
    REQUIRE(size(l0) == 0);
    REQUIRE(size(l1) == 1);
    REQUIRE(size(l2) == 2);
    REQUIRE(size(l3) == 3);
  }
  SECTION("head") {
    REQUIRE(l0 == head(l0));
    REQUIRE(lchar == head(l1));
    REQUIRE(lint == head(l2));
    REQUIRE(lvoid == head(l3));
  }
  SECTION("tail") {
    REQUIRE(l0 == tail(l0));
    REQUIRE(l0 == tail(l1));
    REQUIRE(l1 == tail(l2));
    REQUIRE(l2 == tail(l3));
    REQUIRE(l0 == tail(tail(tail(l3))));
  }
  SECTION("last") {
    REQUIRE(l0 == last(l0));
    // REQUIRE(lchar == last(l1));
    // REQUIRE(lchar == last(l2));
    // REQUIRE(lchar == last(l3));
  }
}
}
