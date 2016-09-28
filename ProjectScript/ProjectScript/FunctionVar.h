#include "Variable.h"
#include "ScriptFunction.h"
#include <functional>

#ifndef FUNCTION_VAR_H
#define FUNCTION_VAR_H

class FunctionVar :
	public Variable
{
public:
	FunctionVar(void);
	~FunctionVar(void);

	Variable * clone(void) const;

	std::shared_ptr<Variable> operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate = true);

	std::shared_ptr<ScriptFunction> script_func;
	std::function<std::shared_ptr<Variable>(const std::vector<std::shared_ptr<Variable>>&)> func_ptr;
};

#endif