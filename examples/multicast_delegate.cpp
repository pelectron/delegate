#include "multicast_delegate.hpp"

#include <iostream>

// custom assert macro
#define assert(expr)                                              \
  if (!!!(expr)) {                                                \
    std::cout << "\nfailed assert on line " << (__LINE__)         \
              << "with expansion:\n"                              \
              << #expr << "\n";                                   \
  } else {                                                        \
    std::cout << "\nassertion successfull on line " << (__LINE__) \
              << " with expansion: " << #expr << "\n";            \
  }

using delegate_t = multicast_delegate<int &(int)>;
using delegate2_t = multicast_delegate<void(void)>;
void free(){}
struct F{
  void operator()(){}
  void member_func(){}
};
inline static int val{15};
int       &free_func(int) { return val; }
struct Functor {
  int &operator()(int a) { return free_func(a); }
  int &member_func(int a) { return free_func(a); }
};

int main() {
  Functor    f;
  delegate_t delegate;
  delegate.bind(&free_func);
  delegate.bind(Functor{});
  delegate.bind(f, &Functor::member_func);
  delegate(0);
  for(const auto& res: delegate){
    assert(res == free_func(0));
  }
  F f2;
  delegate2_t d2;
  d2.bind(&free);
  d2.bind(F{});
  d2.bind(f2,&F::member_func);
  d2();
}