cmake_minimum_required(VERSION 3.10)

set(CUR_PROJ_NAME hook_any_x64)
set(VCPKG_TARGET_TRIPLET x64-mingw-dynamic)

project(${CUR_PROJ_NAME})

# Main library
add_library(${CUR_PROJ_NAME} SHARED
                src/detouracceptor.cpp
                src/fn_hook.cpp
                src/notifydetouracceptor.cpp
                src/patchcode.cpp
                src/sigscan.cpp)
              
target_include_directories(${CUR_PROJ_NAME} PRIVATE include/hook_any_x64)
target_compile_definitions(${CUR_PROJ_NAME} PRIVATE BUILD_HOOKANY64)

find_package(asmjit CONFIG REQUIRED)
target_link_libraries(${CUR_PROJ_NAME} PUBLIC asmjit::asmjit)

find_package(capstone CONFIG REQUIRED)
target_link_libraries(${CUR_PROJ_NAME} PRIVATE capstone::capstone)

# Tests
enable_testing()

function(hook_any_x64_add_test testname)
    add_executable(${testname} test/${testname}.cpp)

    target_include_directories(${testname} PRIVATE include)
    target_link_libraries(${testname} PRIVATE ${CUR_PROJ_NAME})

    add_test(NAME ${testname}
            COMMAND $<TARGET_FILE:${testname}>)
endfunction()

hook_any_x64_add_test(notify_test)
hook_any_x64_add_test(notify_modify_test)
