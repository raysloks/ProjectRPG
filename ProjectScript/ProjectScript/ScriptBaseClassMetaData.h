#pragma once

#include "ScriptVariableMetaData.h"
#include "ScriptFunctionWrapper.h"

#include <map>
#include <string>
#include <functional>

#define RegisterMember(a, b) var.insert(std::make_pair(std::string(#b), ScriptVariableMetaData(NewScriptTypeData(a().b), offsetof(a, b))));
#define RegisterFunction(a, b) func.insert(std::make_pair(std::string(#b), NewScriptFunctionWrapper(&a::b)));
#define RegisterInterface(a, b) incomplete.insert(std::make_pair(std::string(#b), NewScriptFunctionPrototype(&a::b))); // Maybe use function pointers instead? Or a virtual function table?

#define InvokeInterface(b, ...) svftable[meta.incomplete.find(#b) - meta.incomplete.begin()]; //TODO implement

class ScriptBaseClassMetaData
{
public:
	ScriptBaseClassMetaData();
	~ScriptBaseClassMetaData();

	std::map<std::string, ScriptVariableMetaData> var;
	std::map<std::string, ScriptFunctionWrapperBase*> func;
	std::map<std::string, ScriptFunctionPrototypeOld> incomplete;
};

