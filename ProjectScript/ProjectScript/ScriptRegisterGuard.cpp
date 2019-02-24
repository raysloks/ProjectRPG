#include "ScriptRegisterGuard.h"

ScriptRegisterGuard::ScriptRegisterGuard(ScriptCompile * c) : ScriptRegisterGuard(c, c->target.regm)
{
}

ScriptRegisterGuard::ScriptRegisterGuard(ScriptCompile * c, uint8_t r)
{
	comp = c;
	reg = r;
	if (comp->IsFree(reg))
		comp->SetBusy(reg);
	else
		comp = nullptr;
}

ScriptRegisterGuard::~ScriptRegisterGuard()
{
	if (comp != nullptr)
		comp->SetFree(reg);
}