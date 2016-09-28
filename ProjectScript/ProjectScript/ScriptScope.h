#ifndef SCRIPT_SCOPE_H
#define SCRIPT_SCOPE_H

#include "ScriptVariableData.h"

#include <sstream>
#include <map>

class ScriptScope
{
public:
	size_t offset;

	std::map<std::string, ScriptVariableData> vars;
	//std::map<std::string, std::pair<ScriptFunctionPrototype, void*>> funcs;
};

#endif