#ifndef SCRIPT_CLASS_DATA_H
#define SCRIPT_CLASS_DATA_H

#include "ScriptFunctionPrototype.h"
#include "ScriptVariableData.h"

#include <unordered_map>

class ScriptClassData
{
public:
	ScriptClassData();
	~ScriptClassData();

	void AddMember(const std::string& name, ScriptTypeData type);
	void AddFunction(const std::string& name, ScriptFunctionPrototype prototype, void * pointer);

	std::unordered_multimap<std::string, std::pair<ScriptFunctionPrototype, void*>> functions;
	std::unordered_map<std::string, ScriptVariableData> members;
	size_t size;
};

#endif