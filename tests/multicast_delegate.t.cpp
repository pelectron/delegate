#include "multicast_delegate.hpp"
using namespace pc;

int free_func(int a) { return a; }
struct Functor {
  int operator()(int a) { return free_func(a); }
  int member_func(int a) { return free_func(a); }
};
using delegate_t = multicast_delegate<int(int)>;

#include "catch2/catch.hpp"
SCENARIO("testing multicast_delegate") {
  GIVEN("a default constructed delegate") {
    delegate_t del;
    REQUIRE(del.num_callables() == 0);
    REQUIRE(del.delegate_begin() == del.delegate_end());
    REQUIRE(del.begin() == del.end());
    REQUIRE(del.num_results() == 0);
    WHEN("callables are bound to the delegate") {
      Functor f;
      del.bind(&free_func);
      del.bind(Functor{});
      del.bind(f, &Functor::member_func);
      REQUIRE(del.num_callables() == 3);
      REQUIRE(del.num_results() == 0);

      del(5);
      REQUIRE(del.num_results() == 3);
      for (const auto &res : del) {
        REQUIRE(res == free_func(5));
      }

      del(5);
      REQUIRE(del.num_results() == 6);
      for (const auto &res : del) {
        REQUIRE(res == free_func(5));
      }
    }
  }
}