#include "fn_hook.h"

#include "patchcode.h"
#include "notifydetouracceptor.h"

#include <asmjit/asmjit.h>

fn_hook::~fn_hook()
{
	this->code->Disable();
	delete this->acceptor;
	delete this->code;
	delete this->runtime;
}

fn_hook::fn_hook(void* target_fn, void* detour, void* userdata) :
	target_fn(target_fn),
	detour(detour),
	userdata(userdata)
{
	this->runtime = new asmjit::JitRuntime();
	this->code = PatchCode::Create(this->runtime, this->target_fn, NULL);
	this->acceptor = NotifyDetourAcceptor::Create(this->runtime, this->code->UnpatchedFunction(), detour, userdata);
	this->code->SetDestination((uint8_t*)this->acceptor->fn());
}

void fn_hook::Enable()
{
	this->code->Enable();
}

void fn_hook::Disable()
{
	this->code->Disable();
}

void fn_hook::SetEnabled(bool value)
{
	this->code->SetEnabled(value);
}