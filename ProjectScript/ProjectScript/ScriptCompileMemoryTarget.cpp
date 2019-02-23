#include "ScriptCompileMemoryTarget.h"

ScriptCompileMemoryTarget::ScriptCompileMemoryTarget()
{
	lvalue = false;
	mode = 0b11;
	regm = 0b000;
	offset = 0;
}

ScriptCompileMemoryTarget::~ScriptCompileMemoryTarget()
{
}

uint8_t ScriptCompileMemoryTarget::GetModRegRM(uint8_t reg)
{
	return mode << 6 | reg << 3 | regm;
}

bool ScriptCompileMemoryTarget::operator==(const ScriptCompileMemoryTarget& rhs) const
{
	if (lvalue && rhs.lvalue)
		return true;
	return mode == rhs.mode && regm == rhs.regm && offset == rhs.offset;
	// maybe check for unused offset and such?
}

bool ScriptCompileMemoryTarget::operator!=(const ScriptCompileMemoryTarget& rhs) const
{
	if (lvalue != rhs.lvalue)
		return true;
	return mode != rhs.mode || regm != rhs.regm || offset != rhs.offset;
	// maybe check for unused offset and such?
}