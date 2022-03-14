## INTRO
Welcome to this repository. It contains a simple, header only c++ delegate implementation. C++17 or later is required. If you find any bugs, unexpected behavior or failing tests feel free to contact me.

## how to use
The examples folder contains a fully documented example.

## how to include in own projects
Download/clone the git project. Add 'include' folder to your compiler's include path. Add ``#include "delegate.hpp"`` to your sources. Done.

## how to include in meson projects
Either create a meson wrap file, or clone the project into your subprojects folder. Add the following line to your meson.build file: 
``delegate_dep = dependency('delegate', fallback:['delegate', 'delegate_dep'])``

## how to build the project with meson
Clone the project. ``cd`` to the directory containing the meson.build file in your preferred terminal. Enter the following lines:
    ``meson setup build``
    ``meson compile -C build``
Afterwards, the example and the test executable should be in the build folder, if the setup and compile steps were successful. There should not be any build errors with the test, as I have included a copy of catch2 in the repository.

## how to build without meson
The example should be buildable without much hassle. Just add the 'include' folder to the include path and compile 'example/delegate_example.cpp'. 
The test is not simple to build if you don't already have a copy of catch2 and know how to build/link it. Also all that's needed if you have catch2 already, is to add the 'include' folder to the include path, along with the catch2 include path.

## License
This project is distributed under the Boost Software License version 1.0. See LICENSE_1_0.txt for more details.
