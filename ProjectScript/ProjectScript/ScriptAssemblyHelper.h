#ifndef SCRIPT_ASSEMBLY_HELPER_H
#define SCRIPT_ASSEMBLY_HELPER_H

#include <sstream>
#include <vector>

#include "ScriptCompileMemoryTarget.h"

class ScriptCompile;

class ScriptAssemblyHelper
{
public:
	ScriptAssemblyHelper(ScriptCompile& comp);
	~ScriptAssemblyHelper();

	ScriptCompileMemoryTarget FindRegister();
	ScriptCompileMemoryTarget FindRegister(ScriptCompileMemoryTarget& target);
	ScriptCompileMemoryTarget FindRegister(const std::vector<ScriptCompileMemoryTarget>& target);
	void Move(uint8_t opcode, ScriptCompileMemoryTarget& destination, ScriptCompileMemoryTarget& source);
	void Move(uint8_t opcode, uint8_t opcode_extension, ScriptCompileMemoryTarget& operand);
	void Push(ScriptCompileMemoryTarget& source);
	void Pop(ScriptCompileMemoryTarget& destination);

private:
	ScriptCompile& comp;
	std::stringstream& ss;
};

#endif