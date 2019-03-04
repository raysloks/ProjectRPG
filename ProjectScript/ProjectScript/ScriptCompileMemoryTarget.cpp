#include "ScriptCompileMemoryTarget.h"

ScriptCompileMemoryTarget::ScriptCompileMemoryTarget()
{
	lvalue = false;
	mode = 0b11;
	regm = 0b000;
	offset = 0;
}

ScriptCompileMemoryTarget::ScriptCompileMemoryTarget(uint8_t reg)
{
	lvalue = false;
	mode = 0b11;
	regm = reg;
	offset = 0;
}

ScriptCompileMemoryTarget::ScriptCompileMemoryTarget(uint8_t m, uint8_t r, int32_t o)
{
	lvalue = false;
	mode = m;
	regm = r;
	offset = o;
}

ScriptCompileMemoryTarget::~ScriptCompileMemoryTarget()
{
}

uint8_t ScriptCompileMemoryTarget::GetModRegRM(uint8_t reg) const
{
	return (mode << 6) | ((reg & 0b111) << 3) | (regm & 0b111);
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