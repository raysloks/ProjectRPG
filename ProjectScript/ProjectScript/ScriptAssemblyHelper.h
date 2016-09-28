#ifndef SCRIPT_ASSEMBLY_HELPER_H
#define SCRIPT_ASSEMBLY_HELPER_H

#include "ScriptCompile.h"

class ScriptAssemblyHelper
{
public:
	ScriptAssemblyHelper(ScriptCompile& comp);
	~ScriptAssemblyHelper();

	ScriptCompileMemoryTarget FindRegister(const std::vector<ScriptCompileMemoryTarget>& targets);
	void Move(uint8_t opcode, ScriptCompileMemoryTarget& destination, ScriptCompileMemoryTarget& source);
	void Move(uint8_t opcode, uint8_t opcode_extension, ScriptCompileMemoryTarget& operand);
	void Push(ScriptCompileMemoryTarget& source);
	void Pop(ScriptCompileMemoryTarget& destination);

private:
	std::stringstream& ss;
};

#endif