#include "ScriptFunction.h"

ScriptFunction::ScriptFunction(void)
{
}

ScriptFunction::~ScriptFunction(void)
{
}

std::shared_ptr<Variable> ScriptFunction::run(const std::vector<std::shared_ptr<Variable>>& arguments)
{
	std::shared_ptr<ScriptMemory> mem(new ScriptMemory());
	for (int i=0;i<arg.size() && i<arguments.size();++i)
		mem->setVariable(arg[i], arguments[i]);
	return code->run(mem);
}