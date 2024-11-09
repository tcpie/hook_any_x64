# About
Library for easy hooking of arbitrary functions in WIN32 x64 executables.

Only requires target function address. Disassembles the function prolog, so that it can adjust the patch code as necessary.

User-supplied detour functions are provided with the hooked function's parameters, which the user is free to adjust - changes are applied by hook_any_x64 and the hooked function is then called.

See test directory for example code.


# Building
Dependencies:
* [asmjit](https://asmjit.com/)
* [capstone](http://www.capstone-engine.org/)

Compilation is easiest using [cmake and vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started). Execute the following in the top-level project directory:

    cmake --preset=default
    cmake --build build

Tests can then be executed with

    ctest --test-dir ./build


# Limitations
Not thread safe: does not pause any threads while hook is set. The author has not experienced any crashes due to race conditions though...


# License
MIT license (see LICENSE.md). License of respective dependencies apply.
