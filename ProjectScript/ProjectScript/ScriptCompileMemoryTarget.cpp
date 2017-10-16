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

bool ScriptCompileMemoryTarget::operator==(const ScriptCompileMemoryTarget& rhs) const
{
	if (lvalue && rhs.lvalue)
		return true;
	return mod == rhs.mod && rm == rhs.rm && offset == rhs.offset;
	// maybe check for unused offset and such?
}

bool ScriptCompileMemoryTarget::operator!=(const ScriptCompileMemoryTarget& rhs) const
{
	if (lvalue != rhs.lvalue)
		return true;
	return mod != rhs.mod || rm != rhs.rm || offset != rhs.offset;
	// maybe check for unused offset and such?
}