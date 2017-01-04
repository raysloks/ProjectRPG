#ifndef SCRIPT_SCOPE_H
#define SCRIPT_SCOPE_H

#include "ScriptVariableData.h"

#include <sstream>
#include <unordered_map>

class ScriptScope
{
public:
	size_t offset;

	std::unordered_map<std::string, ScriptVariableData> vars;
	std::vector<std::pair<ScriptCompileMemoryTarget, ScriptCompileMemoryTarget>> swaps;
	//std::map<std::string, std::pair<ScriptFunctionPrototype, void*>> funcs;
};

#endif