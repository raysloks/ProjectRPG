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
	ScriptCompileMemoryTarget FindRegister(const ScriptCompileMemoryTarget& target);
	ScriptCompileMemoryTarget FindRegister(const std::vector<ScriptCompileMemoryTarget>& target);
	void Move(uint8_t opcode, const ScriptCompileMemoryTarget& destination, const ScriptCompileMemoryTarget& source);
	void MoveR(uint8_t opcode, uint8_t opcode_extension, const ScriptCompileMemoryTarget& operand);
	void Push(const ScriptCompileMemoryTarget& source);
	void Pop(const ScriptCompileMemoryTarget& destination);

private:
	ScriptCompile& comp;
	std::stringstream& ss;
};

#endif