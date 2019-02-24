#ifndef SCRIPT_COMPILE_MEMORY_TARGET_H
#define SCRIPT_COMPILE_MEMORY_TARGET_H

#include <cstdint>

class ScriptCompileMemoryTarget
{
public:
	ScriptCompileMemoryTarget();
	ScriptCompileMemoryTarget(uint8_t reg);
	~ScriptCompileMemoryTarget();

	uint8_t GetModRegRM(uint8_t reg);

	bool lvalue;

	uint8_t mode, regm;

	int32_t offset;

	bool operator==(const ScriptCompileMemoryTarget& rhs) const;
	bool operator!=(const ScriptCompileMemoryTarget& rhs) const;
};

#endif