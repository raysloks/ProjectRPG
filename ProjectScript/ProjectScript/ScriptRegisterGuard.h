#pragma once

#include "ScriptCompile.h"

class ScriptRegisterGuard
{
public:
	ScriptRegisterGuard(ScriptCompile * comp);
	ScriptRegisterGuard(ScriptCompile * comp, uint8_t reg);
	~ScriptRegisterGuard();

	ScriptCompile * comp;
	uint8_t reg;
};