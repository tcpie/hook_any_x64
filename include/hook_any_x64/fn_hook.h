#ifndef HOOKANY64_FNHOOK_H
#define HOOKANY64_FNHOOK_H

#include "shared.h"
#include <asmjit/asmjit.h>

class PatchCode;
class NotifyDetourAcceptor;

class HOOKANY64_EXPORT fn_hook
{
private:
	void* target_fn;
	void* detour;
	void* userdata;
	asmjit::JitRuntime* runtime;
	PatchCode* code;
	NotifyDetourAcceptor* acceptor;
public:
	~fn_hook();
	fn_hook(void* target_fn, void* detour, void* userdata);

	void Enable();
	void Disable();
	void SetEnabled(bool value);
};

#endif
