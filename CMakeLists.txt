cmake_minimum_required(VERSION 3.10)

set(CUR_PROJ_NAME hook_any_x64)
set(VCPKG_TARGET_TRIPLET x64-mingw-dynamic)

project(CUR_PROJ_NAME)

add_library(CUR_PROJ_NAME
                detouracceptor.cpp
                fn_hook.cpp
                notifydetouracceptor.cpp
                patchcode.cpp
                sigscan.cpp)
                
find_package(asmjit CONFIG REQUIRED)
target_link_libraries(CUR_PROJ_NAME PRIVATE asmjit::asmjit)

find_package(capstone CONFIG REQUIRED)
target_link_libraries(Hooking PRIVATE capstone::capstone)
