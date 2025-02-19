#include "notifydetouracceptor.h"

#include <asmjit/core.h>
#include <asmjit/asmjit.h>
#include <asmjit/x86.h>

using namespace asmjit;
using namespace asmjit::x86;

#define PUSH_MMREG(instance, reg) instance.movq(r11, reg); \
                                  instance.push(r11)
#define POP_MMREG(instance, reg)  instance.pop(r11); \
                                  instance.movq(reg, r11)

NotifyDetourAcceptor::NotifyDetourAcceptor(void* code)
{
    this->code = code;
}

NotifyDetourAcceptor::~NotifyDetourAcceptor()
{
    // TODO: Not sure if we should be free-ing the asm-jit generated code here...
}

void* NotifyDetourAcceptor::fn()
{
    return this->code;
}

NotifyDetourAcceptor* NotifyDetourAcceptor::Create(asmjit::JitRuntime *runtime, void* unhooked_fn, void *callback, void *userdata)
{
    JitRuntime rt;
    CodeHolder code;
    code.init(rt.environment());
    Assembler a(&code);

    // In the stack,
    // we are now at the return pointer.
    // Let's store this
    a.mov(r10,ptr(rsp)); 

    a.push(rbp);
    a.mov(rbp, rsp);
    
    a.push(r10);    // Return address
    a.push(rcx);    // First argument
    a.push(rdx);    // Second argument
    a.push(r8);     // Third argument
    a.push(r9);     // Fourth argument

    PUSH_MMREG(a, xmm0);    // First arg if floating point
    PUSH_MMREG(a, xmm1);    // Second arg if floating point
    PUSH_MMREG(a, xmm2);    // Third arg if floating point
    PUSH_MMREG(a, xmm3);    // Fourth arg if floating point
                            // Additional floating point args are on stack

    a.mov(rdx, rsp);    // Here we assign the second argument
                        // for the callback function.
                        //
                        // We assign the value of RSP, in other words, it
                        // is a pointer to the top of the stack. That is
                        // essentially a struct containing the values of
                        // XMM3...XMM0, R9, R8, RDX, RCX.

    // Store non-volatile registers
    //   These are not used to store call args, but nevertheless these
    //   are non-volatile.
    //      See: https://learn.microsoft.com/en-us/cpp/build/x64-calling-convention?view=msvc-170#callercallee-saved-registers
    // Non volatile args:
    //  RBX, RBP (already stored), RDI, RSI, R12, R13, R14, R15, XMM6-XMM15
    a.push(rbx);
    a.push(rdi);
    a.push(rsi);
    a.push(r12);
    a.push(r13);
    a.push(r14);
    a.push(r15);
    PUSH_MMREG(a, xmm6);
    PUSH_MMREG(a, xmm7);
    PUSH_MMREG(a, xmm8);
    PUSH_MMREG(a, xmm9);
    PUSH_MMREG(a, xmm10);
    PUSH_MMREG(a, xmm11);
    PUSH_MMREG(a, xmm12);
    PUSH_MMREG(a, xmm13);
    PUSH_MMREG(a, xmm14);
    PUSH_MMREG(a, xmm15);

    a.sub(rsp, 40);// Reserve four stack locations for the callee
                   // See: https://docs.microsoft.com/en-us/cpp/build/stack-usage?view=msvc-160#stack-allocation
                   //
                   // 4 * 8 bytes = 32 bytes
                   // For some reason we need to add  8...

    a.mov(rcx, (uint64_t)userdata); // Userdata is the first argument
    a.call((uint64_t)callback);     // Call the callback

    a.add(rsp, 40);// Recover four stack locations

    // Recover saved registers
    POP_MMREG(a, xmm15);
    POP_MMREG(a, xmm14);
    POP_MMREG(a, xmm13);
    POP_MMREG(a, xmm12);
    POP_MMREG(a, xmm11);
    POP_MMREG(a, xmm10);
    POP_MMREG(a, xmm9);
    POP_MMREG(a, xmm8);
    POP_MMREG(a, xmm7);
    POP_MMREG(a, xmm6);

    a.pop(r15);
    a.pop(r14);
    a.pop(r13);
    a.pop(r12);
    a.pop(rsi);
    a.pop(rdi);
    a.pop(rbx);

    // Now recover registers used to store call args
    POP_MMREG(a, xmm3);
    POP_MMREG(a, xmm2);
    POP_MMREG(a, xmm1);
    POP_MMREG(a, xmm0);

    a.pop(r9);
    a.pop(r8);
    a.pop(rdx);
    a.pop(rcx);
    a.pop(r10); // Return address. Popping to fix stack.
    a.pop(rbp);

    // And we jump to the unhooked function
    // Note how any changes to the call arguments, done in the user callback,
    // will be propagated to the target function. See also test/notify_modify_test.cpp.
    //
    // todo: we can probably upgrade this in a smart way to also support changing the 
    //       return value.
    a.mov(asmjit::x86::r11,(uint64_t)unhooked_fn); // R11 register is volatile, so we can use it.
    a.jmp(asmjit::x86::r11);

    void* fn;
    asmjit::Error err = runtime->add(&fn,&code);

    return new NotifyDetourAcceptor(fn);
}
