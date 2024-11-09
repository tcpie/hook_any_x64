#include "detouracceptor.h"

#include <vector>
#include <string>
#include <iostream>

using namespace asmjit;
using namespace std;

DetourAcceptor::DetourAcceptor(void* code)
{
    this->code = code;
}

DetourAcceptor* DetourAcceptor::Create(asmjit::JitRuntime* runtime, size_t num_args, asmjit::TypeId* arg_types, asmjit::TypeId ret_type, void* callback, void* userdata)
{
    asmjit::CodeHolder code_;
    code_.init(runtime->environment());

    asmjit::x86::Compiler c(&code_);

    asmjit::FuncSignatureBuilder sig(asmjit::CallConvId::kX64Windows);

    sig.setRet(ret_type);

    for (size_t i = 0; i < num_args; i++) {
        sig.addArg(arg_types[i]);
    }

    asmjit::FuncNode* fn_node = c.addFunc(sig);

    // Create the args
    vector<asmjit::x86::Gp> args;

    for (size_t i = 0; i < num_args; i++) {
        // Add registers
        asmjit::x86::Gp temp = c.newGp(arg_types[i]);

        fn_node->setArg(i,temp);
        args.push_back(temp);
    }

    // TODO:
    // Somehow pass the arguments inside
    // an array / set structure

    // Also create the args for the callback
    // function
    asmjit::FuncSignatureBuilder sig_callback(asmjit::CallConvId::kX64Windows);

    sig_callback.setRet(ret_type);

    sig_callback.addArg(asmjit::TypeId::kUInt64);

    for (size_t i = 0; i < num_args; i++) {
        sig_callback.addArg(arg_types[i]);
    }

    InvokeNode* invoke_node;

    // Call the callback
    c.invoke(&invoke_node, (uint64_t)callback,sig_callback);

    invoke_node->setArg(0,asmjit::imm((uint64_t)userdata));

    for (size_t i = 0; i < num_args; i++) {
        invoke_node->setArg(i+1,args[i]);
    }

    asmjit::x86::Gp ret_reg = c.newGp(ret_type);
    invoke_node->setRet(0,ret_reg);

    c.ret(ret_reg);
    c.endFunc();
    c.finalize();

    void* fn;
    asmjit::Error err = runtime->add(&fn,&code_);

    return new DetourAcceptor(fn);
}

void* DetourAcceptor::fn()
{
    return this->code;
}
