#ifndef NOTIFYDETOURACCEPTOR_H
#define NOTIFYDETOURACCEPTOR_H

#include "shared.h"

#include <stdint.h>

namespace asmjit
{
    class JitRuntime;
}

class ANY_HOOK NotifyDetourAcceptor
{
private:
    void* code;
    NotifyDetourAcceptor(void* code);

public:
    union xmm
    {
        double d;
        float f;
    };
    struct call_state
    {
        xmm xmm3;
        xmm xmm2;
        xmm xmm1;
        xmm xmm0;
        uint64_t r9;
        uint64_t r8;
        uint64_t rdx;
        uint64_t rcx;
        void* ret_addr;
    };

    ~NotifyDetourAcceptor();

    void* fn();

    static NotifyDetourAcceptor* Create(asmjit::JitRuntime *runtime, void* unhooked_fn, void *callback, void *userdata);
};

#endif // NOTIFYDETOURACCEPTOR_H
