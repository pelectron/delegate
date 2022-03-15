#include "multicast_delegate.hpp"

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
    THEN("the delegate has no callables bound to it") {
      REQUIRE(del.num_callables() == 0);
    }
    THEN("The delegate has no results stored") {
      REQUIRE(del.begin() == del.end());
      REQUIRE(del.num_results() == 0);
    }
    WHEN("callables are bound") {
      Functor f;
      del.bind(&free_func);
      del.bind(Functor{});
      del.bind(f, &Functor::member_func);
      THEN("num_callables() returns the correct number of callables bound") {
        REQUIRE(del.num_callables() == 3);
      }
      AND_THEN("num_results() is still 0") { REQUIRE(del.num_results() == 0); }
      AND_THEN("invoking the delegate adds to return values to its result vector"){
        del(5);
        REQUIRE(del.num_results() == 3);
        for(const auto& res: del){
          REQUIRE(res == free_func(5));
        }
        THEN("invoking the delegate a second time doubles the number of stored results"){
          del(5);
          REQUIRE(del.num_results() == 6);
        }
        for(const auto& res: del){
          REQUIRE(res == free_func(5));
        }
      }
    }
  }
}