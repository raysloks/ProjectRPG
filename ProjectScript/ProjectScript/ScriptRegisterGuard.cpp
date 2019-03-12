#include "ScriptRegisterGuard.h"

ScriptRegisterGuard::ScriptRegisterGuard(ScriptCompile * c, uint8_t r)
{
	comp = c;
	reg = r;
	comp->SetBusy(reg);
}

ScriptRegisterGuard::~ScriptRegisterGuard()
{
	comp->SetFree(reg);
}