#include "delegate.hpp"
#include "multicast_delegate.hpp"

#include <iostream>
#include <string>
using namespace pc;

static constexpr int expected = 2;
int                &&free_func() {
  static int r{expected};
  return std::move(r);
}

struct Functor {
  int &&operator()() { return free_func(); }
};

#include "catch2/catch.hpp"

SCENARIO("testing r value reference return type") {
  GIVEN("a delegate") {
    delegate<int && (void)> del(&free_func);
    auto                    val = std::move(del());
    REQUIRE(val == expected);
    del.bind(Functor{});
    val = std::move(del());
    REQUIRE(val == expected);
  }
  GIVEN("a multicast_delegate") {
    multicast_delegate<int && (void)> del;
    Functor                           f;
    del.bind(&free_func);
    del.bind(Functor{});
    del.bind(f);
    del();
    for (auto &result : del) {
      REQUIRE(result == expected);
    }
  }
}