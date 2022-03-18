# read me
Welcome to this repository. It contains a header only c++17 delegate implementation. A delegate is used to invoke free functions, member functions and function objects in a uniform manner. It contains two classes: delegate, which can only bind to a single callable, and multicast_delegate, which can bind multiple callables and collect their return values.
If you find any bugs, unexpected behavior or failing tests feel free to contact me.

# how to use
The examples folder contains fully documented examples of the delegate and multicast_delegate class. 

Short version:

Suppose we have a free function.
```cpp 
int calc(int i) { return i; }
```

And some class which has the same interface.
```cpp 
class MyClass{
  int calc(int i){ return 3*i; }
};
```

And a big function object.
```cpp
 auto lambda = [/*some big capture > 16 bytes*/](int i)->int { 
   // very important work with the big capture before returning
   return 2*i; 
   };
```

With the delegate class, it is possible to execute them in a uniform manner.
```cpp
#include "delegate.hpp"
#include <iostream>
#include <assert>
int main(){
  // create a delegate with the right signature
  pc::delegate<int(int)> delegate;
  // bind it to a free function
  delegate.bind(&calc);
  assert(delegate(5) == 5);

  MyClass c;
  // rebind delegate to a member function
  delegate.bind(c, &MyClass::calc);
  assert(delegate(5) == 15);

  // rebind to lambda:
  delegate.bind(lambda);
  assert(delegate(5) == 10);

  // reset the delegate. This unbinds the currently bound callable.
  delegate.reset();
  assert(delegate.is_valid() == false); 
  return 0;
}
```

With the multicast_delegate class it is possible to bind and execute multiple callables and 'collect' their results.
```cpp
#include "multicast_delegate.hpp"
#include <iostream>
#include <assert>
int main(){
  MyClass c;
  pc::multicast_delegate<int(int)> multi_delegate;
  multi_delegate.bind(&calc);
  multi_delegate.bind(c, &MyClass::calc);
  multi_delegate.bind(lambda);

  // multi_delegate now has three callable. Invoking multi_delegate yields three results. They results are in the same order as the callables bound, i.e. result 0 is the result of the free function calc, result 1 is the result of the calc call on variable c and result 2 the result of calling lambda.
  multi_delegate(5);

  // iterating over the results can be done with a simple ranged for loop.
  for(int result: multi_delegate){
    std::cout << "result number " << i " = " << result << '\n';
  }

  // calling the delegate again doubles the result size, as old results don't get cleared. 
  multi_delegate(1); // now six results contained in multi_delegate. The three first ones are the old results.
  for(int result: multi_delegate){
    std::cout << "result number " << i " = " << result << '\n';
  }

  // get the number of results 
  std::cout << "number of results = " << multi_delegate.num_results() << ".\n";
  // get the number of callables bound
  std::cout << "number of callables bound = " << multi_delegate.num_callables() << ",\n";

  // clearing the results. Leaves the callables bound untouched.
  multi_delegate.clear_results();
  std::cout << "number of results (after clear_results()) = " << multi_delegate.num_results() << ".\n";
  std::cout << "number of callables bound (after clear_results()) = " << multi_delegate.num_callables() << ",\n";
  multi_delegate(13);

  // resetting unbinds the callables, but leaves the results untouched.
  multi_delegate.reset();
  std::cout << "number of results (after reset()) = " << multi_delegate.num_results() << ".\n";
  std::cout << "number of callables bound (after reset()) = " << multi_delegate.num_callables() << ",\n";

  // total reset clear both the results and unbinds all bound callables.
  multi_delegate.total_reset();
  std::cout << "number of results (after total_reset()) = " << multi_delegate.num_results() << ".\n";
  std::cout << "number of callables bound (after total_reset()) = " << multi_delegate.num_callables() << ",\n";
}
```

# how to include in your own projects
Download/clone the git project. Add 'include' folder to your compiler/build system/whatever you use include path. Add ``#include "delegate.hpp"`` or ``#include "multicast_delegate.hpp"``to your sources.

# how to include in meson projects
Either create a meson wrap file, or clone the project into your subprojects folder. Add the following line to your meson.build file: 
``delegate_dep = dependency('delegate', fallback:['delegate', 'delegate_dep'])``

And then use it in some executable/library/whatever you are trying to create:
``my_exe = executable('my_exe', my_sources, dependencies:[delegate_dep,...], ...)``

# how to build the project with meson
Clone the project. ``cd`` to the directory containing the meson.build file in your preferred terminal. Enter the following lines:
  - ``meson setup build``
  - ``meson compile -C build``
  
Afterwards, the example and the test executable should be in the build folder, if the setup and compile steps were successful. There should not be any build errors with the test, as I have included a copy of catch2 in the repository.

# how to build without meson
The example should be buildable without much hassle. Just add the 'include' folder to the include path and compile the files in the examples folder. They are standalone, so no linking or extra steps required.
The test is not simple to build if you don't already have a copy of catch2 and know how to build/link against it.I have however included a copy of catch2 in the subprojects folder.

# License
This project is distributed under the Boost Software License version 1.0. See LICENSE_1_0.txt for more details.
