#include <archie/function_chain.hpp>
#include <catch.hpp>

namespace {
using namespace archie;

TEST_CASE("Can use function chain", "[function_chain]") {
  SECTION("Can create empty chain") {
    auto chain = function_chain<int()>{};
    REQUIRE(chain.size() == 0);
  }

  SECTION("Can push back lambda to chain") {
    auto chain = function_chain<int()>{};
    chain.push_back([]() { return 7; });
    REQUIRE(chain.size() == 1);
  }

  SECTION("Can call functions in chain") {
    auto chain = function_chain<int()>{};
    chain.push_back([]() { return 3; });
    chain.push_back([]() { return 5; });
    chain.push_back([]() { return 7; });
    int i = 0;
    for (auto const& f : chain) { i += f(); }
    REQUIRE(i == 15);
  }
}

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
}
}
