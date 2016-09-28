#include "ScriptCode.h"

#ifndef SCRIPT_FUNCTION_H
#define SCRIPT_FUNCTION_H

class ScriptFunction
{
public:
	ScriptFunction(void);
	~ScriptFunction(void);

	std::shared_ptr<Variable> run(const std::vector<std::shared_ptr<Variable>>& arguments);

	std::shared_ptr<ScriptCode> code;
	std::vector<std::string> arg;
};

#endif