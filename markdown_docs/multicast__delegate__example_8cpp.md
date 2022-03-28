---
title: C:/Users/pelec/OneDrive/Dokumente/Programming/C++Libs/delegate/examples/multicast_delegate_example.cpp

---

# C:/Users/pelec/OneDrive/Dokumente/Programming/C++Libs/delegate/examples/multicast_delegate_example.cpp



## Functions

|                | Name           |
| -------------- | -------------- |
| int | **[main](multicast__delegate__example_8cpp.md#function-main)**() |


## Functions Documentation

### function main

```cpp
int main()
```




## Source code

```cpp

#include "../include/multicast_delegate.hpp"

#include <iostream>

// custom assert macro
// prints the expr in string form and the line number if it evaluates to false
// to std out
#define assert(expr)                                      \
  if (!bool(expr)) {                                      \
    std::cout << "\nfailed assert on line " << (__LINE__) \
              << "with expansion: '" << #expr << "'\n";   \
  }

struct F {
  void operator()() {}
  void member_func() {}
};

int free_func() { return 0; }

struct Functor {
  int operator()() { return 1; }
  int member_func() { return 2; }
};

using namespace pc;

int main() {
  Functor                       f;
  multicast_delegate<int(void)> delegate; // multicast_delegate instance

  // binding multiple callables to the multicast_delegate
  delegate.bind(&free_func); // binding a free function
  delegate.bind(Functor{});  // binding a function object
  delegate.bind(f,
                &Functor::member_func); // binding a object an member function
  // the delegate should now be bound to four callables.
  assert(delegate.num_callables() == 3);

  // calling the multicast_delegate. Should now contain three results.
  delegate();
  assert(delegate.num_results() == 3);
  std::cout << "called delegate for the first time:\nnumber of results = "
            << delegate.num_results()
            << "\nnumber of callables = " << delegate.num_callables() << "\n";
  // the results can now be easily iterated in a range based for loop
  int i = 0;
  for (const auto &res : delegate) {
    std::cout << "result number " << i++ << " = " << res << '\n';
  }

  // calling multicast_delegate again without clearing the results, the number
  // of results have doubled
  delegate();
  assert(delegate.num_results() == 6);
  std::cout << "called delegate for the second time with out clearing the "
               "results:\nnumber of results = "
            << delegate.num_results()
            << "\nnumber of callables = " << delegate.num_callables() << "\n";
  i = 0;
  for (const auto &res : delegate) {
    std::cout << "result number " << i++ << " = " << res << '\n';
  }

  // clearing the results vector. The results vector is now empty. The callables
  // bound before are still bound to the multicast_delegate after a call to
  // clear_results.
  delegate.clear_results();
  assert(delegate.num_results() == 0);
  assert(delegate.num_callables() == 3);

  // clearing the multicast_delegate's delegate vector. Now there are no
  // callables bound to the multicast_delegate anymore.
  delegate.reset();
  assert(delegate.num_callables() == 0);
}
```


-------------------------------

Updated on 2022-03-28 at 10:54:23 +0200
