---
title: readme.in.md

---

# readme.in.md






## Source code

```markdown
# About
Welcome to this repository. It contains a header only c++17 delegate implementation. A delegate is used to invoke free functions, member functions and function objects in a uniform manner. Two classes are provided:
 - [delegate], which can only bind to a single callable
 - and [multicast_delegate], which can bind multiple callables and collect their return values.


The docs are hosted [here](https://pelectron.github.io/delegate).
If you find any bugs, unexpected behavior, failing tests, general feedback or improvements on the code, feel free to contact me at pelectron1602@gmail.com, make a pull request or start an issue. It would be awesome to get some feedback.

# How to use
The examples directory contains fully documented examples of the [delegate] and [multicast_delegate] class. Here is a short version:

Suppose we have a free function:

```cpp 
int calc(int i) { return i; }
```

And some class which has the same interface:

```cpp 
class MyClass{
  int calc(int i){ return 3*i; }
};
```

And some function object with the same calling signature:

```cpp
 auto lambda = [](int i)->int { 
   // very important work with the big capture before returning
   return 2*i; 
   };
```

With the [delegate] class, it is possible to execute them in a uniform manner.

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

With the [multicast_delegate] class it is possible to bind and execute multiple callables and 'collect' their results.

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
  return 0;
}
```

# How to include in your own projects
1. <a href="https://github.com/pelectron/delegate/archive/refs/heads/master.zip">**Download the zip**</a> or <a href="https://github.com/pelectron/delegate.git">**clone**</a> the git project. 
2. Add the 'include' directory to your compiler's or build system's include path. 
3. Add ``#include "delegate.hpp"`` or ``#include "multicast_delegate.hpp"`` to your sources.

# How to include in meson projects
Either create a meson wrap file, or clone the project into your subprojects directory. Add the following line to your meson.build file: 

```python
delegate_dep = dependency('delegate', fallback:['delegate', 'delegate_dep'])
```
Then use it in some executable/library/whatever you are trying to create, e.g. 
```python 
my_exe = executable('my_exe', my_sources, dependencies:[delegate_dep,...], ...)
```

# How to build the examples and tests with meson
``cd`` into the directory containing the ``meson.build`` file in your preferred terminal. Enter the following lines

```shell 
meson setup build
```

If this was successful, compile with

```shell 
meson compile -C build
```

Afterwards, the examples and the test executable should be in the ``build`` directory, if the setup and compile steps were successful. There should not be any build errors with the tests, as I have included a copy of <a href ="https://github.com/catchorg/Catch2.git">catch2</a> in this repository. The two example executables will be named ``delegate_example`` and ``multicast_delegate_example``. The test executables are called ``test_debug`` and ``test_release``.

# How to build the examples and tests without meson
To use the library, no actual building is required. However, building and debugging the examples and tests to understand the underlying code can be helpful and fun, so I try to provide instructions for doing so with only the command line and a compiler. It may not perfectly work on your system and need some adjustment. I assume you have clang installed. If not, you will probably have to change the flag syntax.
Firstly, change into the directory containing this readme file and setup your build directory.

```shell
mkdir build
```

Building the examples in debug mode:

```shell
clang++ -std=c++17 -g examples/delegate_example.cpp -o build/delegate_example
clang++ -std=c++17 -g examples/multicast_delegate_example.cpp -o build/multicast_delegate_example
```

on windows with clang:

```shell
clang++ -std=c++17 -g examples/delegate_example.cpp -o build/delegate_example.exe
clang++ -std=c++17 -g examples/multicast_delegate_example.cpp -o build/multicast_delegate_example.exe
```

This gives you two executables in the build directory called delegate_example and multicast_delegate_example.
Building the debug mode test is similar:

```shell

clang++ -std=c++17 -g tests/delegate.t.cpp tests/multicast_delegate.t.cpp tests/r_value_ref_test.cpp tests/test_main.cpp -Iinclude -Isubprojects/Catch2-2.13.7/single_include -o build/test_debug
```

Again on windows with clang:

```shell
clang++ -std=c++17 -g tests/delegate.t.cpp tests/multicast_delegate.t.cpp tests/r_value_ref_test.cpp tests/test_main.cpp -Iinclude -Isubprojects/Catch2-2.13.7/single_include -o build/test_debug.exe
```

And also the release version for the test:

```shell
clang++ -std=c++17 -O3 tests/delegate.t.cpp tests/multicast_delegate.t.cpp tests/r_value_ref_test.cpp tests/test_main.cpp -Iinclude -Isubprojects/Catch2-2.13.7/single_include -o build/test_release
```
This should give you a executable called test_debug and test_release in the build directory. If you run them, all tests should pass.

# How to build the documentation
This project is documented with doxygen. By default, using the doxyfile contained in this repo will build the html version of the documentation in the docs/html subdirectory. Use the doxy wizard or doxygen from the command line with ``doxygen Doxyfile`` if doxygen is available in your path. The index.html should be now be in the docs/html directory. Alternatively, you can use meson to build it. To do this, you must 
1. run ``meson configure build -Dbuild_docs=enabled`` in the project root directory,
2. then change into the build directory with ``cd build`` 
3. and lastly actually build the docs with ``ninja docs``

Another way to build the doc is to invoke the python build script in the root directory with ``python build_docs.py``

# Contributing
I welcome every pull request gladly. Please just make sure to format your pull request with the .clang-format file of this project. If you add a new feature, please also write a test for it with <a href ="https://github.com/catchorg/Catch2.git">catch2</a> and add the test sources to the tests subdirectory along with appropriate documentation.

# License
This project is distributed under the Boost Software License version 1.0. See LICENSE_1_0.txt for more details.

# Online Documentation
Hosted [here](https://pelectron.github.io/delegate).
```


-------------------------------

Updated on 2022-03-28 at 10:41:56 +0200
