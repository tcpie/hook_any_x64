#ifndef HOOKANY64_DETOURACCEPTOR_H
#define HOOKANY64_DETOURACCEPTOR_H

#include "shared.h"
#include <asmjit/asmjit.h>

class HOOKANY64_EXPORT DetourAcceptor
{
private:
    void* code;

    DetourAcceptor(void* code);
public:

    void* fn();
    //static void Crash(asmjit::JitRuntime* runtime, void* callback);
    static DetourAcceptor* Create(asmjit::JitRuntime* runtime, size_t num_args, asmjit::TypeId* arg_types, asmjit::TypeId ret_type, void* callback, void* userdata);
};

#endif // DETOURACCEPTOR_H
