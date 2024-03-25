#include "notifydetouracceptor.h"

#include "asmjit/core.h"
#include "asmjit/asmjit.h"
#include "asmjit/x86.h"

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
    a.mov(r10,rsp);

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
                            // Additiona floating point args are on stack

    a.mov(rdx, rsp);    // Here we assign the second argument
                        // for the callback function

    a.sub(rsp, 40);// Reserve four stack locations for the callee
                   // See: https://docs.microsoft.com/en-us/cpp/build/stack-usage?view=msvc-160#stack-allocation
                   //
                   // 4 * 8 bytes = 32 bytes
                   // For some reason we need to add  8...

    a.mov(rcx, (uint64_t)userdata); // Userdata is the first argument
    a.call((uint64_t)callback);               // Call the callback

    a.add(rsp, 40);// Recover four stack locations

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

    a.mov(asmjit::x86::r11,(uint64_t)unhooked_fn); // R11 register is volatile, so we can use it.
    a.jmp(asmjit::x86::r11);

    void* fn;
    asmjit::Error err = runtime->add(&fn,&code);

    return new NotifyDetourAcceptor(fn);
}
