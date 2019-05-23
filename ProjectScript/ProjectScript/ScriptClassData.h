#ifndef SCRIPT_CLASS_DATA_H
#define SCRIPT_CLASS_DATA_H

#include "ScriptFunctionPrototype.h"
#include "ScriptVariableData.h"
#include "ScriptVirtualFunctionData.h"

#include <unordered_map>
#include <optional>

class ScriptClassData
{
public:
	ScriptClassData();
	ScriptClassData(const std::string& name, size_t size, bool dynamic = false);
	~ScriptClassData();

	void SetParent(const std::shared_ptr<ScriptClassData>& ptr);

	void AddVirtualFunctionTable();

	void AddMember(const std::string& name, const ScriptTypeData& type);
	void AddMember(const std::string& name, const ScriptTypeData& type, int32_t offset);
	void AddFunction(const std::string& name, const ScriptFunctionPrototype& prototype, void * pointer);
	void AddVirtualFunction(const std::string& name, const ScriptFunctionPrototype& prototype);

	template <typename R, typename T, typename ...Args>
	void AddFunction(const std::string& name, R(T::*f)(Args...))
	{
		AddFunction(name, NewScriptFunctionPrototype(f), *(void**)&f);
	}

	ScriptVariableData GetMember(const std::string& name) const;
	void GetMemberList(std::vector<ScriptVariableData>& list) const;
	void * GetFunctionFinalAddress(const std::string& name, const ScriptFunctionPrototype& prototype) const;
	std::optional<ScriptFunctionPrototype> GetFunctionFullPrototype(const std::string& name, const ScriptFunctionPrototype& prototype) const;
	off_t GetVirtualFunctionIndex(const std::string& name, const ScriptFunctionPrototype& prototype) const;
	void GetVirtualFunctionList(std::vector<ScriptVirtualFunctionData>& list) const;

	size_t GetVirtualFunctionCount() const;

	std::string class_name;
	std::shared_ptr<ScriptClassData> parent;
	std::unordered_multimap<std::string, std::pair<ScriptFunctionPrototype, void*>> functions;
	std::vector<ScriptVirtualFunctionData> virtual_functions;
	std::unordered_map<std::string, ScriptVariableData> members;
	size_t size;
	void * vftable;
};

#endif