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
}
