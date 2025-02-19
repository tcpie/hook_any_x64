#include "patchcode.h"

#include <string.h>
#include <asmjit/asmjit.h>
#include <asmjit/x86.h>
#include <capstone/capstone.h>

#include <iostream>

#include <windows.h>

using namespace std;

PatchCode::PatchCode(asmjit::JitRuntime* runtime,
                     void *function_address,
                     void* destination,
                     void* patch_code,
                     void *unpatched_fn,
                     size_t patch_size,
                     size_t org_code_size,
                     size_t unpatched_fn_size)
{
    this->runtime = runtime;
    this->function_address = (uint8_t*)function_address;
    this->destination = (uint8_t*)destination;
    this->patch_code = (uint8_t*)patch_code;
    this->patch_size = patch_size;
    this->org_code_size = org_code_size;
    this->org_code = (uint8_t*)unpatched_fn;
    this->unpatched_fn = (uint8_t*)unpatched_fn;
    this->unpatched_fn_size = unpatched_fn_size;
    this->is_enabled = false;
}

PatchCode::~PatchCode()
{
    this->Disable();
    free(this->unpatched_fn);
    this->runtime->release(this->patch_code);
}

void PatchCode::SetDestination(uint8_t* value)
{
    bool was_enabled = this->is_enabled;

    if (was_enabled)
        this->Disable();

    this->destination = value;

    // Free our previous patch code
    this->runtime->release(this->patch_code);

    this->create_jmp(this->runtime,
                     this->function_address,
                     this->destination,
                     (void**)&this->patch_code,
                     &this->patch_size);

    if (was_enabled)
        this->Enable();
}

int PatchCode::count_num_start_bytes(void *function, size_t patch_size)
{
    csh handle;

    if (cs_open(CS_ARCH_X86, CS_MODE_64, &handle) != CS_ERR_OK)
        return -1;

    cs_insn* insn = cs_malloc(handle);

    uint8_t* code = (uint8_t*)function;
    uint64_t address = (uint64_t)code;
    size_t size = patch_size * 4;
    size_t found_num_bytes = 0;

    while (cs_disasm_iter(handle, (const uint8_t**)&code, &size, &address, insn)) {
            found_num_bytes += insn->size;

            if (found_num_bytes >= patch_size) {
                cs_free(insn, 1);
                cs_close(&handle);
                return found_num_bytes;
            }
    }

    cs_free(insn, 1);
    cs_close(&handle);

    return -2;
}

void PatchCode::create_jmp(asmjit::JitRuntime* runtime, void* base_address, void* abs_dest, void **code, size_t *size)
{
    if (code == nullptr || runtime == nullptr) {
        if (size != nullptr)
            *size = 0;
        return;
    }

    asmjit::CodeHolder code_;
    code_.init(runtime->environment(), (uint64_t)base_address);

    asmjit::x86::Assembler a(&code_);

    // a.jmp((uint64_t)abs_dest); // <-- This approach works as well, but not tested for longer offsets

    a.long_().mov(asmjit::x86::r11,(uint64_t)abs_dest); // R11 register is volatile, so we can use it.
    a.jmp(asmjit::x86::r11);

    asmjit::Error err = runtime->add(code,&code_); // Here we are allocating the code, so we should free it somewhere as well...!
    size_t code_size = (size_t)a.offset();
    *size = code_size;

    return;
}


PatchCode* PatchCode::Create(asmjit::JitRuntime* runtime, void *function_address, void *destination)
{
    // We first create the patch code
    void* patch_code = nullptr;
    size_t patch_size = 0;

    // This is the jump from the original function to the destination
    PatchCode::create_jmp(runtime, function_address, destination, &patch_code, &patch_size);

    // Now we see if we can find enough bytes at the start of the function
    //  We do this *after* creating the patch code, as now we know the size of the jump code
    int n_bytes = PatchCode::count_num_start_bytes(function_address, patch_size);

    // If we didn't find enough space, we end up in the next if statement.
    //  todo: free patch code?
    if (n_bytes <= 0)
        return nullptr;

    // "unpatched_fn" : trampoline back to the original function
    void* unpatched_fn = (uint8_t*)malloc(n_bytes + patch_size*3); // Reserve some extra space, in case this jmp is larger.
    memcpy(unpatched_fn, function_address, n_bytes); // Put in original instructions.

    void* jmp_code = nullptr;
    size_t jmp_code_size = 0;

    // This jump is from the trampoline, back into the original code
    PatchCode::create_jmp(runtime,
                         (void*)((uint64_t)unpatched_fn + n_bytes),
                         (void*)((uint64_t)function_address + n_bytes),
                         &jmp_code,
                         &jmp_code_size);

    // todo: check that indeed this jump fits in the space we allocated for it inside our unpatched_fn buffer
    memcpy((void*)((uint64_t)unpatched_fn + n_bytes), jmp_code, jmp_code_size);

    runtime->release(jmp_code);

    DWORD old_prot = 0;
    VirtualProtect((LPVOID)unpatched_fn, n_bytes + jmp_code_size, PAGE_EXECUTE_READWRITE, &old_prot);

    return new PatchCode(runtime,
                         function_address,
                         destination,
                         patch_code,
                         unpatched_fn,
                         patch_size,
                         n_bytes,
                         n_bytes + jmp_code_size);
}

bool PatchCode::IsEnabled()
{
    return this->is_enabled;
}

void PatchCode::SetEnabled(bool value)
{
    if (value == true)
        this->Enable();
    else
        this->Disable();
}

void PatchCode::Enable()
{
    DWORD old_prot = 0;

    VirtualProtect((LPVOID)this->function_address, this->patch_size, PAGE_EXECUTE_READWRITE, &old_prot);

    memcpy(this->function_address, this->patch_code, this->patch_size);

    this->is_enabled = true;

    VirtualProtect((LPVOID)this->function_address, this->patch_size, old_prot, &old_prot);
}

void PatchCode::Disable()
{
    DWORD old_prot = 0;

    VirtualProtect((LPVOID)this->function_address, this->org_code_size, PAGE_EXECUTE_READWRITE, &old_prot);

    memcpy(this->function_address, this->org_code, this->org_code_size);

    VirtualProtect((LPVOID)this->function_address, this->org_code_size, old_prot, &old_prot);

    this->is_enabled = false;
}

size_t PatchCode::PatchSize()
{
    return this->patch_size;
}

size_t PatchCode::OrgCodeSize()
{
    return this->org_code_size;
}

uint8_t* PatchCode::Patch()
{
    return this->patch_code;
}

uint8_t* PatchCode::Destination()
{
    return this->destination;
}

uint8_t* PatchCode::FunctionAddress()
{
    return this->function_address;
}

void* PatchCode::UnpatchedFunction()
{
    return this->unpatched_fn;
}
