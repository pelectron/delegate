/**
 * @file delegate_example.cpp
 * @author Pele Constam (pele.constam@gmail.com)
 * @brief This file contains an example of how to use the delegate class.
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
// custom assert macro
#define assert(expr)                                                         \
  if (!!!(expr)) {                                                           \
    std::cout << "\nfailed assert on line " << (__LINE__) << "with expansion:\n" \
              << #expr << "\n";                                              \
  } else {                                                                   \
    std::cout << "\nassertion successfull on line " << (__LINE__)              \
              << " with expansion: " << #expr << "\n";                       \
  }

// below are some example free functions, custom function objects and lambdas to
// show how to use a delegate. In this example, all function like things to
// invoke will have a parameter list of int,float and return type float.

float free_func(int a, float b) { return a * b; }

struct SmallFunctor {
  float operator()(int a, float b) { return a * b; }
};

class BigFunctor {
public:
  BigFunctor() = default;
  constexpr BigFunctor(float f, size_t s) : data{0.0}, n(s) {
    for (size_t i = 0; i < s; ++i) {
      for (size_t j = 0; j < s; ++j) {
        data[i] *= f;
      }
    }
  }

  float operator()(int a, float b) const { return calculate(a, b); }

  float member_func(int a, float b) const { return calculate(a, b); }

private:
  float calculate(int a, float b) const {
    float ret = 0.0;
    for (size_t i = 0; i < n; ++i) {
      ret += data[i] * a + b;
    }
    return ret;
  }
  float  data[16]{0};
  size_t n{5};
};
auto lambda = [](int a, float f) -> float { return a + f; };
auto lambda2 = lambda;

int main() {
  delegate<float(int, float)> my_delegate; // creating an empty delegate
  assert(my_delegate.is_valid() == false); // should be invalid

  my_delegate.bind(&free_func); // binding a free function to it
  // invoking the delegate gives the same result as calling the free function
  // directly
  assert(my_delegate(5, 10.0f) == free_func(5, 10.0f));

  // binding small function objects/functors -> stored inline
  SmallFunctor small_functor;
  my_delegate.bind(SmallFunctor{});
  assert(my_delegate(5, 10.f) == small_functor(5, 10.0f));

  // binding small lambda, which is essentially the same case as the small
  // functor
  my_delegate.bind(lambda);
  assert(my_delegate(5, 10.0f) == lambda2(5, 10.0f));

  // binding to a big function object (does heap allocation)
  BigFunctor big_functor;
  my_delegate.bind(BigFunctor{});
  assert(my_delegate(5, 10.0f) == big_functor(5, 10.0f));

  // binding to member function via a reference to the object and address of its
  // member function
  my_delegate.bind(big_functor, &BigFunctor::member_func);
  assert(my_delegate(5, 10.0f) == big_functor.member_func(5, 10.0f));

  // delegates can be reset, and are invalid afterwards
  my_delegate.reset();
  assert(my_delegate.is_valid() == false);
  // Invoking a invalid delegate does not rely on/cause undefined behaviour.
  // In case of a non void return value, a 0 initialized value should be
  // returned
  assert(my_delegate(5, 10.0f) == 0.0f);
}