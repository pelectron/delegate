/**
 * @file delegate.t.cpp
 * @author Pele Constam (pelectron1602@gmail.com)
 * @brief This file contains the test for the delegate class.
 * @version 0.1
 * @date 2022-03-14
 * We need to test a few things:
 * 1. constructors -> done
 * 2. binding callables and during that allocations -> done
 * 3. reset operation -> done
 * 4. copy and move construction -> done
 * 5. copy and move assignment -> done
 * 6. invocation -> done
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

template <typename T>
T free_f_t(T a) {
  return a;
}
template <typename T>
struct Small_t {
  Small_t() : copied(false), moved(false) {}
  Small_t(const Small_t &other) : copied(true), moved(false) {}
  Small_t(Small_t &&other) : copied(false), moved(true) {}
  T    member_func(T a) { return a; }
  T    const_member_func(T a) const { return a; }
  T    operator()(T a) { return a; }
  T    noexcept_member_func(T a) { return a; }
  bool copied;
  bool moved;
};
// big callable structure, i.e. bigger than inlinebale size

template <typename T>
struct Big_t : public Small_t<T> {
  char buf[pc::impl::max_storage_size + 24]{0};
};

#include "catch2/catch.hpp"
TEMPLATE_TEST_CASE("delegate constructor", "[delegate constructor] [template]",
                   int, float, double, char, unsigned) {
  GIVEN("a declared delegate") {
    delegate<TestType(TestType)> delegate;
    THEN("the delegate is invalid") { REQUIRE_FALSE(delegate.is_valid()); }
    THEN("the returned value is 0  initialized") {
      REQUIRE(delegate(TestType{42}) == TestType{0});
    }
  }
  GIVEN("a delegate constructed from free function") {
    AllocCounter                 c;
    delegate<TestType(TestType)> delegate(&free_f_t<TestType>);
    bool                         alloc_happend = c.alloc_happend();
    THEN("the delegate does not allocate and returns expected results") {
      REQUIRE_FALSE(alloc_happend);
      REQUIRE(delegate.is_valid());
      REQUIRE(delegate(TestType{10}) == TestType{10});
      REQUIRE(delegate(TestType{42}) == TestType{42});
    }
  }
  GIVEN("a delegate constructed from member function") {
    Small_t<TestType>            small;
    AllocCounter                 c;
    delegate<TestType(TestType)> delegate(small,
                                          &Small_t<TestType>::member_func);
    bool                         alloc_happend = c.alloc_happend();
    THEN("the delegate does not allocate and returns expected results") {
      REQUIRE_FALSE(alloc_happend);
      REQUIRE(delegate.is_valid());
      REQUIRE(delegate(TestType{10}) == TestType{10});
      REQUIRE(delegate(TestType{42}) == TestType{42});
    }
  }
  GIVEN("a delegate constructed from const member function") {
    Small_t<TestType>            small;
    AllocCounter                 c;
    delegate<TestType(TestType)> delegate(
        small, &Small_t<TestType>::const_member_func);
    bool alloc_happend = c.alloc_happend();
    THEN("the delegate does not allocate and returns expected results") {
      REQUIRE_FALSE(alloc_happend);
      REQUIRE(delegate.is_valid());
      REQUIRE(delegate(TestType{10}) == TestType{10});
      REQUIRE(delegate(TestType{42}) == TestType{42});
    }
  }
  GIVEN("a delegate constructed from small function object") {
    Small_t<TestType>            small;
    AllocCounter                 c;
    delegate<TestType(TestType)> delegate(small);
    bool                         alloc_happend = c.alloc_happend();
    THEN("the delegate does not allocate and returns expected results") {
      REQUIRE_FALSE(alloc_happend);
      REQUIRE(delegate.is_valid());
      REQUIRE(delegate(TestType{10}) == TestType{10});
      REQUIRE(delegate(TestType{42}) == TestType{42});
    }
  }
  GIVEN("a delegate constructed from small function object") {
    Small_t<TestType>            small;
    AllocCounter                 c;
    delegate<TestType(TestType)> delegate(small);
    bool                         alloc_happend = c.alloc_happend();
    THEN("the delegate does not allocate and returns expected results") {
      REQUIRE_FALSE(alloc_happend);
      REQUIRE(delegate.is_valid());
      REQUIRE(delegate(TestType{10}) == TestType{10});
      REQUIRE(delegate(TestType{42}) == TestType{42});
    }
  }
  GIVEN("a delegate constructed from big function object") {
    Big_t<TestType>              big;
    AllocCounter                 c;
    delegate<TestType(TestType)> delegate(big);
    bool                         alloc_happend = c.alloc_happend();
    THEN("the delegate allocates and returns expected results") {
      REQUIRE(alloc_happend);
      REQUIRE(delegate.is_valid());
      REQUIRE(delegate(TestType{10}) == TestType{10});
      REQUIRE(delegate(TestType{42}) == TestType{42});
    }
  }
}

TEMPLATE_TEST_CASE("delegate copying", "[delegate copy construct] [template]",
                   int, float, double, char, unsigned) {
  GIVEN("an invalid delegate") {
    delegate<TestType(TestType)> d1;
    WHEN("copying it") {
      delegate<TestType(TestType)> d2(d1);
      THEN("both are invalid and return the same value") {
        REQUIRE_FALSE(d1.is_valid());
        REQUIRE_FALSE(d2.is_valid());
        REQUIRE(d1(TestType{42}) == d2(TestType{42}));
      }
    }
  }
  GIVEN("a delegate copy constructed from another") {
    Small_t<TestType>            small;
    delegate<TestType(TestType)> d1(small);
    delegate<TestType(TestType)> d2(d1);
    THEN("both are valid and return the same value") {
      REQUIRE(d1.is_valid());
      REQUIRE(d2.is_valid());
      REQUIRE(d1(TestType{10}) == d2(TestType{10}));
      REQUIRE(d1(TestType{42}) == d2(TestType{42}));
    }
  }
}

TEMPLATE_TEST_CASE("delegate moving", "[delegate move construct] [template]",
                   int, float, double, char, unsigned) {
  GIVEN("a delegate move constructed from another") {
    AllocCounter                 ca;
    DeAllocCounter               cd;
    delegate<TestType(TestType)> d2(
        std::move(delegate<TestType(TestType)>{Small_t<TestType>{}}));
    bool alloc_happend = ca.alloc_happend() || cd.dealloc_happend();
    THEN("the delegate is valid and returns expected results") {
      REQUIRE_FALSE(alloc_happend);
      REQUIRE(d2.is_valid());
      Small_t<TestType> small;
      REQUIRE(small(TestType{10}) == d2(TestType{10}));
      REQUIRE(small(TestType{42}) == d2(TestType{42}));
    }
  }
  GIVEN("a delegate move constructed from another containing a big function object") {
    delegate<TestType(TestType)> d1{Big_t<TestType>{}};
    AllocCounter                 ca;
    DeAllocCounter               cd;
    delegate<TestType(TestType)> d2(std::move(d1));
    bool alloc_happend = ca.alloc_happend() || cd.dealloc_happend();
    THEN("the delegate is valid and returns expected results") {
      REQUIRE_FALSE(alloc_happend);
      REQUIRE(d2.is_valid());
      Big_t<TestType> big;
      REQUIRE(big(TestType{10}) == d2(TestType{10}));
      REQUIRE(big(TestType{42}) == d2(TestType{42}));
    }
  }
}

TEMPLATE_TEST_CASE("delegate moving assignment",
                   "[delegate move assignment] [template]", int, float, double,
                   char, unsigned) {
  GIVEN("a delegate move assigned from another") {
    AllocCounter                 ca;
    DeAllocCounter               cd;
        delegate<TestType(TestType)> d1{Small_t<TestType>{}};
    delegate<TestType(TestType)> d2;
    d2 = std::move(d1);
    bool alloc_happend = ca.alloc_happend() || cd.dealloc_happend();
    THEN("the delegate is valid and returns expected results") {
      REQUIRE_FALSE(alloc_happend);
      REQUIRE(d2.is_valid());
      Small_t<TestType> small;
      REQUIRE(small(TestType{10}) == d2(TestType{10}));
      REQUIRE(small(TestType{42}) == d2(TestType{42}));
    }
  }
  GIVEN("a delegate move assigned from another containing a big function object") {
    delegate<TestType(TestType)> d1{Big_t<TestType>{}};
    AllocCounter                 ca;
    DeAllocCounter               cd;
        delegate<TestType(TestType)> d2;
    d2 = std::move(d1);
    bool alloc_happend = ca.alloc_happend() || cd.dealloc_happend();
    THEN("the delegate is valid and returns expected results") {
      REQUIRE_FALSE(alloc_happend);
      REQUIRE(d2.is_valid());
      Big_t<TestType> big;
      REQUIRE(big(TestType{10}) == d2(TestType{10}));
      REQUIRE(big(TestType{42}) == d2(TestType{42}));
    }
  }
}

TEMPLATE_TEST_CASE("delegate reset", "[delegate reset] [template]", int, float,
                   double, char, unsigned) {
  GIVEN("an invalid delegate") {
    delegate<TestType(TestType)> delegate;
    REQUIRE_FALSE(delegate.is_valid());
    WHEN("resetting it") {
      delegate.reset();
      THEN("the delegate is still invalid") {
        REQUIRE_FALSE(delegate.is_valid());
      }
    }
  }
  GIVEN("a delegate bound to member function") {
    Small_t<TestType>            small;
    delegate<TestType(TestType)> delegate(small,
                                          &Small_t<TestType>::member_func);
    REQUIRE(delegate.is_valid());
    WHEN("resetting it") {
      DeAllocCounter c;
      delegate.reset();
      bool dealloc_happend = c.dealloc_happend();
      THEN("the delegate is invalid") {
        REQUIRE_FALSE(delegate.is_valid());
        REQUIRE_FALSE(dealloc_happend);
      }
    }
  }
  GIVEN("a delegate bound to const member function") {
    Small_t<TestType>            small;
    delegate<TestType(TestType)> delegate(
        small, &Small_t<TestType>::const_member_func);
    REQUIRE(delegate.is_valid());
    WHEN("resetting it") {
      DeAllocCounter c;
      delegate.reset();
      bool dealloc_happend = c.dealloc_happend();
      THEN("the delegate is invalid") {
        REQUIRE_FALSE(delegate.is_valid());
        REQUIRE_FALSE(dealloc_happend);
      }
    }
  }
  GIVEN("a delegate bound to small function object") {
    delegate<TestType(TestType)> delegate(Small_t<TestType>{});
    REQUIRE(delegate.is_valid());
    WHEN("resetting it") {
      DeAllocCounter c;
      delegate.reset();
      bool dealloc_happend = c.dealloc_happend();
      THEN("the delegate is invalid") {
        REQUIRE_FALSE(delegate.is_valid());
        REQUIRE_FALSE(dealloc_happend);
      }
    }
  }
  GIVEN("a delegate bound to big function object") {
    delegate<TestType(TestType)> delegate(Big_t<TestType>{});
    REQUIRE(delegate.is_valid());
    WHEN("resetting it") {
      DeAllocCounter c;
      delegate.reset();
      bool dealloc_happend = c.dealloc_happend();
      THEN("the delegate is invalid and deallocates") {
        REQUIRE_FALSE(delegate.is_valid());
        REQUIRE(dealloc_happend);
      }
    }
  }
}

