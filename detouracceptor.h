#ifndef DETOURACCEPTOR_H
#define DETOURACCEPTOR_H

#include "shared.h"
#include "asmjit/asmjit.h"

class ANY_HOOK DetourAcceptor
{
private:
    void* code;

    DetourAcceptor(void* code);
public:

    void* fn();
    //static void Crash(asmjit::JitRuntime* runtime, void* callback);
    static DetourAcceptor* Create(asmjit::JitRuntime* runtime, size_t num_args, asmjit::Type::Id* arg_types, asmjit::Type::Id ret_type, void* callback, void* userdata);
};

#endif // DETOURACCEPTOR_H
