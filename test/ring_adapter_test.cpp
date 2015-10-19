#include <utility>
#include <archie/container/ring_adapter.hpp>

#include <algorithm>
#include <catch.hpp>
#include <resource.hpp>
namespace {
using namespace archie;
TEST_CASE("ring_adapter", "[ring]") {
  using buff_t = std::vector<test::resource>;
  using ring_t = ring_adapter<buff_t>;
  buff_t buff;
  buff.reserve(4);
  ring_t ring{std::move(buff)};
  REQUIRE(ring.empty());
  REQUIRE(ring.size() == 0);
  REQUIRE(std::distance(std::begin(ring), std::end(ring)) == ring.size());

  ring.emplace_back(0);
  REQUIRE(!ring.empty());
  REQUIRE(ring.size() == 1);
  REQUIRE(std::distance(std::begin(ring), std::end(ring)) == ring.size());
  REQUIRE(*std::begin(ring) == 0);

  ring.emplace_back(1);
  REQUIRE(ring.size() == 2);
  REQUIRE(std::distance(std::begin(ring), std::end(ring)) == ring.size());
  REQUIRE(*std::begin(ring) == 0);

  ring.emplace_back(2);
  REQUIRE(ring.size() == 3);
  REQUIRE(std::distance(std::begin(ring), std::end(ring)) == ring.size());
  REQUIRE(*std::begin(ring) == 0);

  ring.emplace_back(3);
  REQUIRE(ring.size() == 4);
  REQUIRE(std::distance(std::begin(ring), std::end(ring)) == ring.size());
  REQUIRE(*std::begin(ring) == 0);

  ring.emplace_back(3);
  REQUIRE(ring.size() == 4);
  REQUIRE(std::distance(std::begin(ring), std::end(ring)) == ring.size());
  REQUIRE(*std::begin(ring) == 1);
}
}
