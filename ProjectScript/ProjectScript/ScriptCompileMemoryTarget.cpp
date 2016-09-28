#include "ScriptCompileMemoryTarget.h"

ScriptCompileMemoryTarget::ScriptCompileMemoryTarget()
{
	lvalue = false;
	mod = 0b11;
	rm = 0b000;
	offset = 0;
}

ScriptCompileMemoryTarget::~ScriptCompileMemoryTarget()
{
}

uint8_t ScriptCompileMemoryTarget::GetModRegRM(uint8_t reg)
{
	return mod << 6 | reg << 3 | rm;
}