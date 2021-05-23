# About
Library for easy hooking of arbitrary functions in WIN32 x64 executables.

Only requires target function address. Disassembles the function prolog using the capstone library, so that it can adjust the patch code as necessary.

User-supplied detour functions are provided with the hooked function's parameters, which the user is free to adjust - changes are applied by hook_any_x64 and the hooked function is then called.

Example code to be added.

# Building
Run the following commands to fetch asmjit:
    git submodule init
    git submodule update

Open the project's directory in Visual Studio Code (configured with the C/C++ module) and compile.

Note: although the build process is setup for MSVC++, it should also work (when adapted) under other compilers such as G++.


# Limitations
Not thread safe: does not pause any threads while hook is set. The author has not experienced any crashes due to race conditions though...

# License
See Licenses subdirectory. In short:

hook_any_x64: MIT license.
capstone: MIT-like license.
asmjit: zlib license.
