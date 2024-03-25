#ifndef HOOKANY64_PATCHCODE_H
#define HOOKANY64_PATCHCODE_H

#include "shared.h"

#include <cstdint>
#include <asmjit/asmjit.h>

//! Hot-patch code generated for a detour
//!
//! The hot-patch code consists of a simple jmp-instruction, which
//! will re-route the code execution to a new destination.
class HOOKANY64_EXPORT PatchCode
{
private:
    uint8_t* function_address;
    uint8_t* destination;

    uint8_t* org_code;
    uint8_t* patch_code;
    uint8_t* unpatched_fn;

    size_t org_code_size;
    size_t patch_size;
    size_t unpatched_fn_size;

    bool is_enabled;

    asmjit::JitRuntime* runtime;

    PatchCode(asmjit::JitRuntime* runtime,
              void* function_address,
              void* destination,
              void* patch_code,
              void* unpatched_fn,
              size_t patch_size,
              size_t org_code_size,
              size_t unpatched_fn_size);

    static void create_jmp(asmjit::JitRuntime* runtime, void* function_address, void *destination, void** code, size_t* size);
    static int count_num_start_bytes(void* function, size_t patch_size);

public:
    //! Public destructor
    ~PatchCode();

    //! Creates a new PatchCode instance
    //!
    //! @param runtime The asmjit runtime to be used for patch creation.
    //! @param function_address The address of the function to be patched.
    //! @param destination The address to which the execution should be re-routed to.
    static PatchCode* Create(asmjit::JitRuntime* runtime, void* function_address, void* destination);

    //! The unpatched function.
    //!
    //! A generated trampoline which can be called as if it is the original, unpatched
    //! function.
    void* UnpatchedFunction();

    //! The address of the patched function.
    uint8_t* FunctionAddress();

    //! Sets the destination to detour to
    //!
    //! If enabled, disables the patch, then generates a new patch code,
    //! then enables the patch again. If disabled, just a new patch code
    //! is generated.
    void SetDestination(uint8_t *value);

    //! The new destination address.
    uint8_t* Destination();

    //! Buffer containing the patch code.
    uint8_t* Patch();

    //! Size of the patch code, in bytes.
    size_t PatchSize();

    //! Size of the affected code of the original function, in bytes.
    //!
    //! When the patch is applied, the code at the start of the function has to be
    //! overwritten. To avoid splitting instructions (which often consist of multiple
    //! bytes), the start of the function is analyzed and the amount of bytes needed
    //! to save the original code is determined. This is why the patch size may be
    //! different from the original code size.
    size_t OrgCodeSize();

    //! Enables the patch.
    void Enable();

    //! Disables the patch; restores the original code.
    void Disable();

    //! Returns a value indicating if the patch is applied.
    //!
    //! @return A value indicating if the patch is currently applied.
    bool IsEnabled();

    //! Sets or removes the patch
    //!
    //! @param value Value indicating if the patch should be applied (True) or
    //!  disabled (False)
    void SetEnabled(bool value);
};

#endif // PATCHCODE_H
