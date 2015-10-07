#include <archie/pure_function.hpp>
#include <catch.hpp>

namespace {
using namespace archie;

int foo(int) { return 3; }
struct bar {
  int operator()(int) const { return 42; };
};
TEST_CASE("pure_function", "[pure_function]") {
  pure_function<int(int)> p1(foo);
  auto boo = [](int) { return 4; };
  pure_function<int(int)> p2(boo);
  bar b;
  pure_function<int(int)> p3(b);
  REQUIRE(p1(2) == foo(2));
  REQUIRE(p2(2) == boo(2));
  REQUIRE(p3(2) == b(2));
  p1 = b;
  p2 = foo;
  p3 = boo;
  REQUIRE(p1);
  p1 = nullptr;
  REQUIRE(!p1);
  using ptr_t = int (*)(int);
  ptr_t ptr = p2;
  auto p4 = make_pure_function<int(int)>(foo);
  ptr_t p5 = make_pure_function<int(int)>(b);
  REQUIRE(p4);
  REQUIRE(p5);
}
}
