#pragma once

#include "shared.h"

namespace asmjit
{
class JitRuntime;
}

class PatchCode;
class NotifyDetourAcceptor;

class ANY_HOOK fn_hook
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

