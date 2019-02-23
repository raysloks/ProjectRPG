#ifndef SCRIPT_CLASS_DATA_H
#define SCRIPT_CLASS_DATA_H

#include "ScriptFunctionPrototype.h"
#include "ScriptVariableData.h"
#include "ScriptVirtualFunctionData.h"

#include <unordered_map>

class ScriptClassData
{
public:
	ScriptClassData();
	~ScriptClassData();

	void SetParent(const std::shared_ptr<ScriptClassData>& ptr);

	void AddVirtualFunctionTable();

	void AddMember(const std::string& name, const ScriptTypeData& type);
	void AddFunction(const std::string& name, const ScriptFunctionPrototype& prototype, void * pointer);
	void AddVirtualFunction(const std::string& name, const ScriptFunctionPrototype& prototype);

	ScriptVariableData GetMember(const std::string& name);
	void * GetFunctionFinalAddress(const std::string& name, const ScriptFunctionPrototype& prototype);
	off_t GetVirtualFunctionIndex(const std::string& name);

	size_t GetVirtualFunctionCount();

	std::string class_name;
	std::shared_ptr<ScriptClassData> parent;
	std::unordered_multimap<std::string, std::pair<ScriptFunctionPrototype, void*>> functions;
	std::vector<ScriptVirtualFunctionData> virtual_functions;
	std::unordered_map<std::string, ScriptVariableData> members;
	size_t size;
};

#endif