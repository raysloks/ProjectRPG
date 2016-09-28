#ifndef SCRIPT_COMPILE_MEMORY_TARGET_H
#define SCRIPT_COMPILE_MEMORY_TARGET_H

#include <cstdint>

class ScriptCompileMemoryTarget
{
public:
	ScriptCompileMemoryTarget();
	~ScriptCompileMemoryTarget();

	uint8_t GetModRegRM(uint8_t reg);

	bool lvalue;

	uint8_t mod, rm;

	uint32_t offset;
};

#endif