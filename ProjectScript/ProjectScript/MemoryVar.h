#include "Variable.h"
#include "ScriptMemory.h"

#ifndef MEMORY_VAR_H
#define MEMORY_VAR_H

class MemoryVar :
	public Variable
{
public:
	MemoryVar(ScriptMemory * memory);
	~MemoryVar(void);

	Variable * clone(void) const;

	std::shared_ptr<Variable> operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate = true);

	ScriptMemory * mem;
};

#endif