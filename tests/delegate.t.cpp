/**
 * @file delegate.t.cpp
 * @author Pele Constam (pelectron1602@gmail.com)
 * @brief This file contains the test for the delegate class.
 * @version 0.1
 * @date 2022-03-14
 *
 * Copyright Pele Constam 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 */
#include "delegate.hpp"

#include <iostream>

using namespace pc;

/// helper to determine allocation counts
class AllocCounter {
public:
  static inline size_t alloc_count{0};

  AllocCounter() : start(alloc_count) {}
  bool   alloc_happend() const { return count() != 0; }
  size_t count() const { return alloc_count - start; }
  void   reset() { start = alloc_count; }
  void   print_state() const {
    if (alloc_happend()) {
      std::cout << "# of allocs: " << count() << "\n";
    } else {
      std::cout << "No allocation\n";
    }
  }

private:
  size_t start;
};
/// helper to determine deallocation counts
class DeAllocCounter {
public:
  static inline size_t dealloc_count{0};

  DeAllocCounter() : start(dealloc_count) {}
  bool   dealloc_happend() const { return count() != 0; }
  size_t count() const { return dealloc_count - start; }
  void   reset() { start = dealloc_count; }
  void   print_state() const {
    if (dealloc_happend()) {
      std::cout << "# of allocs: " << count() << "\n";
    } else {
      std::cout << "No allocation\n";
    }
  }

private:
  size_t start;
};

void *operator new(size_t n) {
  ++AllocCounter::alloc_count;
  return malloc(n);
}
void operator delete(void *p) noexcept {
  ++DeAllocCounter::dealloc_count;
  free(p);
}

// the delegate type used in this test
using Del = delegate<int(int)>;
// free function matching delegate signature
inline int f(int a) { return 2 * a; }
// noexcept version of f
inline int f_noexcept(int a) noexcept { return 2 * a; }

// struct with a public non const member function matching the delegate
// signature
struct S {
  int member_func(int a) { return 3 * a; }
};
// same as struct S, except that the member function is const
struct ConstS {
  int member_func(int a) const { return 3 * a; }
};
// noexcept version of S
struct NoExceptS {
  int member_func(int a) noexcept { return 3 * a; }
};
// const version of NoExceptS
struct ConstNoExceptS {
  int member_func(int a) const noexcept { return 3 * a; }
};
// big callable structure, i.e. much bigger than inlinebale size
struct Big {
  int  operator()(int a) { return a; }
  char buf[100];
};
// const version of Big
struct ConstBig {
  int  operator()(int a) const { return a; }
  char buf[100];
};
// noexcept version of Big
struct NoExceptBig {
  int  operator()(int a) noexcept { return a; }
  char buf[100];
};
// const version of NoExceptBig
struct ConstNoExceptBig {
  int  operator()(int a) const noexcept { return a; }
  char buf[100];
};
// small enough callable to be inlined
struct SmallEnough {
  // SmallEnough()=default;
  // SmallEnough(SmallEnough&&)=default;
  int  operator()(int a) const { return a; }
  char buf[16];
};
// big moveable funcition object
struct BigMove {
  BigMove() = default;
  BigMove(const BigMove &) = default;
  BigMove(BigMove &&other) : moved_to(true), moved_from(false) {
    other.moved_from = true;
    other.moved_to = false;
  }
  int  operator()(int a) { return a; }
  bool moved_to{false};
  bool moved_from = {false};
  char buf[100]{0};
};
#include "catch2/catch.hpp"

SCENARIO("Resetting a empty delegate has no effect") {
  GIVEN("A default constructed delegate") {
    Del del;
    REQUIRE_FALSE(del.is_valid());
    WHEN("resetting the delegate") {
      del.reset();
      THEN("the delegate is still empty") { REQUIRE_FALSE(del.is_valid()); }
    }
  }
}

SCENARIO("Resetting a bound delegate sets the delegate as invalid") {
  GIVEN("A bound delegate") {
    Del del(&f);
    WHEN("resetting the delegate") {
      del.reset();
      THEN("the delegate is invalid") { REQUIRE_FALSE(del.is_valid()); }
    }
  }
}

SCENARIO("Construction and delegation of a free function") {
  AllocCounter c;
  GIVEN("a free function") {
    WHEN("Constructing a delegate from it") {
      c.reset();
      Del  del(&f);
      bool b = c.alloc_happend(); // write it like this
      THEN("the delegate does not allocate") { REQUIRE_FALSE(b); }
      AND_THEN("the delegate is valid") { REQUIRE(del.is_valid()); }
      AND_THEN("Calling the delegate results in the same value as a calling "
               "the function directly") {
        REQUIRE(del(5) == f(5));
      }
    }
  }
}

SCENARIO("Construction and delegation of a non const member function") {
  GIVEN("A class with a non const member function and a instance of it") {
    S instance;
    WHEN("Constructing a delegate from it") {
      AllocCounter c;
      Del          del(instance, &S::member_func);
      bool         b = c.alloc_happend();
      THEN("the delegate does not allocate") { REQUIRE_FALSE(b); }
      AND_THEN("The delegate is valid") { REQUIRE(del.is_valid()); }
      AND_THEN("Calling the delegate results in the same value as a calling "
               "the function") {
        REQUIRE(del(5) == instance.member_func(5));
      }
    }
  }
}

SCENARIO("Construction and delegation of a const member function") {
  GIVEN("A class with a const member function and a instance of it") {
    ConstS const_inst;
    WHEN("Constructing a delegate from it") {
      AllocCounter c;
      Del          del(const_inst, &ConstS::member_func);
      bool         b = c.alloc_happend();
      THEN("the delegate does not allocate") { REQUIRE_FALSE(b); }
      AND_THEN("the delegate is valid") { REQUIRE(del.is_valid()); }
      AND_THEN("Calling the delegate results in the same value as a calling "
               "the function") {
        REQUIRE(del(5) == const_inst.member_func(5));
      }
    }
  }
}

SCENARIO("Construction and delegation of functors") {
  AllocCounter c;
  GIVEN("a small enough functor") {
    SmallEnough small;
    WHEN("constructing a delegate from it") {
      c.reset();
      Del  del(SmallEnough{});
      bool b = c.alloc_happend();
      THEN("the delegate does not allocate") { REQUIRE_FALSE(b); }
      AND_THEN("The delegate is valid") { REQUIRE(del.is_valid()); }
      AND_THEN("calling the delegate results in the same value as a calling "
               "the function") {
        REQUIRE(del(5) == small(5));
      }
    }
  }

  GIVEN("A large functor") {
    ConstBig big;
    WHEN("constructing a delegate from it") {
      c.reset();
      Del  del(big);
      bool b = c.alloc_happend();
      THEN("the delegate allocates") { REQUIRE(b); }
      AND_THEN("the delegate is valid") { REQUIRE(del.is_valid()); }
      AND_THEN("calling the delegate results in the same value as a calling "
               "the function") {
        REQUIRE(del(5) == big(5));
      }
    }
  }
}

SCENARIO("delegates can be moved") {
  AllocCounter   a_cnt;
  DeAllocCounter d_cnt;
  GIVEN("A delegate bound to a big functor") {
    Big big;
    Del del(big);
    WHEN("moving the delegate") {
      a_cnt.reset();
      d_cnt.reset();
      Del  del2 = std::move(del);
      bool allocated = a_cnt.alloc_happend();
      THEN("the new delegate did not allocate") { REQUIRE_FALSE(allocated); }
      AND_THEN("the original delegate is invalid") {
        REQUIRE_FALSE(del.is_valid());
      }
      AND_THEN("the new delegate is valid") { REQUIRE(del2.is_valid()); }
      AND_THEN("calling the new delegates results in the same return value as "
               "calling the function directly") {
        REQUIRE(big(5) == del2(5));
      }
    }
  }
  GIVEN("a delegate bound to a small functor or (member) function pointer") {
    SmallEnough small;
    Del         del(small);
    WHEN("moving the delegate") {
      a_cnt.reset();
      d_cnt.reset();
      Del  del2 = std::move(del);
      bool no_de_or_alloc =
          (not a_cnt.alloc_happend()) && (not d_cnt.dealloc_happend());
      THEN("no memory de/allocation took place") { REQUIRE(no_de_or_alloc); }
      THEN("the original delegate is invalid") {
        REQUIRE_FALSE(del.is_valid());
      }
      AND_THEN("the new delegate is valid") { REQUIRE(del2.is_valid()); }
      AND_THEN("calling the new delegates results in the same return value as "
               "calling the function directly") {
        REQUIRE(small(5) == del2(5));
      }
    }
  }
}

SCENARIO("Binding to noexcept functions or functors") {
  GIVEN("A empty delegate") {
    Del del;
    WHEN("binding to a noexcept free function") {
      del.bind(&f_noexcept);
      THEN("the delegate is valid") { REQUIRE(del.is_valid()); }
      THEN("The delegate can be called") { REQUIRE(del(5) == f_noexcept(5)); }
    }
    WHEN("binding to a non const noexcept member function") {
      NoExceptS s;
      del.bind(s, &NoExceptS::member_func);
      THEN("the delegate is valid") { REQUIRE(del.is_valid()); }
      THEN("The delegate can be called") {
        REQUIRE(del(5) == s.member_func(5));
      }
    }
    WHEN("binding to a const noexcept member function") {
      ConstNoExceptS s;
      del.bind(s, &ConstNoExceptS::member_func);
      THEN("the delegate is valid") { REQUIRE(del.is_valid()); }
      THEN("The delegate can be called") {
        REQUIRE(del(5) == s.member_func(5));
      }
    }
    WHEN("binding to a non const noexcept functor") {
      NoExceptBig big;
      del.bind(big);
      THEN("the delegate is valid") { REQUIRE(del.is_valid()); }
      THEN("The delegate can be called") { REQUIRE(del(5) == big(5)); }
    }
    WHEN("binding to a const noexcept functor") {
      ConstNoExceptBig big;
      del.bind(big);
      THEN("the delegate is valid") { REQUIRE(del.is_valid()); }
      THEN("The delegate can be called") { REQUIRE(del(5) == big(5)); }
    }
  }
}

SCENARIO("moveable function objects can be moved into a delegate") {
  GIVEN("a empty delegate and moveable functor") {
    Del     del;
    BigMove moveable;
    REQUIRE(((moveable.moved_from == false) && (moveable.moved_to == false)));
    WHEN("moving the functor") {
      del.bind(BigMove{});
      THEN("the functor has been moved") {
        // REQUIRE(moveable.moved_from == true);
      }
      AND_THEN("the delegate is valid") { REQUIRE(del.is_valid()); }
      AND_THEN("the delegate works as expected") {
        BigMove m2;
        REQUIRE(m2(5) == del(5));
      }
    }
  }
}
