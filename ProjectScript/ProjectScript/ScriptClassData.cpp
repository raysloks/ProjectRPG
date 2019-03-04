#include "ScriptClassData.h"

ScriptClassData::ScriptClassData()
{
	size = 0;
	vftable = nullptr;
}

ScriptClassData::~ScriptClassData()
{
}

void ScriptClassData::SetParent(const std::shared_ptr<ScriptClassData>& ptr)
{
	parent = ptr;
	size = parent->size;
}

void ScriptClassData::AddVirtualFunctionTable()
{
	if (GetMember("_vfptr") == ScriptVariableData())
	{
		ScriptTypeData type;
		type.type = ST_VOID;
		type.indirection = 2;
		AddMember("_vfptr", type);
	}
}

void ScriptClassData::AddMember(const std::string& name, const ScriptTypeData& type)
{
	ScriptVariableData member_data;
	member_data.type = type;
	
	member_data.target.lvalue = false; // shouldn't really matter, should it?
	member_data.target.offset = size;
	member_data.target.mode = 0b01;
	if (member_data.target.offset > 127)
		member_data.target.mode = 0b10;
	member_data.target.regm = 0b011; // rbx

	members.insert(std::make_pair(name, member_data));
	size_t member_size = type.GetSize();
	size += member_size;
	if (member_size % 8 != 0)
		size += 8 - member_size % 8;
}

void ScriptClassData::AddFunction(const std::string& name, const ScriptFunctionPrototype& prototype, void * pointer)
{
	functions.insert(std::make_pair(class_name + "::" + name, std::make_pair(prototype, pointer)));
}

void ScriptClassData::AddVirtualFunction(const std::string& name, const ScriptFunctionPrototype& prototype)
{
	if (GetMember("_vfptr") == ScriptVariableData())
		throw std::runtime_error("Only dynamic classes may have virtual functions.");
	ScriptVirtualFunctionData func;
	func.name = name;
	func.offset = GetVirtualFunctionCount() * 8;
	func.prototype = prototype;
	virtual_functions.push_back(func);
}

ScriptVariableData ScriptClassData::GetMember(const std::string& name) const
{
	for (auto& member : members)
	{
		if (member.first.compare(name) == 0)
		{
			return member.second;
		}
	}
	if (parent)
		return parent->GetMember(name);
	return ScriptVariableData();
}

void ScriptClassData::GetMemberList(std::vector<ScriptVariableData>& list) const
{
	list.reserve(list.size() + members.size());
	for (auto kv : members)
		list.push_back(kv.second);
	if (parent)
		parent->GetMemberList(list);
}

void * ScriptClassData::GetFunctionFinalAddress(const std::string& name, const ScriptFunctionPrototype& prototype) const
{
	for (auto& func : functions)
	{
		// fully qualified function name
		if (func.first.compare(name) == 0 && func.second.first == prototype)
		{
			return func.second.second;
		}
	}
	for (auto& func : functions)
	{
		if (func.first.compare(class_name + "::" + name) == 0 && func.second.first == prototype)
		{
			return func.second.second;
		}
	}
	if (parent)
		return parent->GetFunctionFinalAddress(name, prototype);
	return nullptr;
}

std::optional<ScriptFunctionPrototype> ScriptClassData::GetFunctionFullPrototype(const std::string& name, const ScriptFunctionPrototype& prototype) const
{
	for (auto& func : functions)
	{
		// fully qualified function name
		if (func.first.compare(name) == 0 && func.second.first.params == prototype.params)
		{
			return func.second.first;
		}
	}
	for (auto& func : functions)
	{
		if (func.first.compare(class_name + "::" + name) == 0 && func.second.first.params == prototype.params)
		{
			return func.second.first;
		}
	}
	if (parent)
		return parent->GetFunctionFullPrototype(name, prototype);
	return std::optional<ScriptFunctionPrototype>();
}

off_t ScriptClassData::GetVirtualFunctionIndex(const std::string& name, const ScriptFunctionPrototype& prototype) const
{
	for (size_t i = 0; i < virtual_functions.size(); i++)
	{
		auto& func = virtual_functions[i];
		if (func.name.compare(name) == 0 && func.prototype == prototype)
		{
			if (parent)
				return i + parent->GetVirtualFunctionCount();
			return i;
		}
	}
	if (parent)
		return parent->GetVirtualFunctionIndex(name, prototype);
	return -1;
}

void ScriptClassData::GetVirtualFunctionList(std::vector<ScriptVirtualFunctionData>& list) const
{
	if (parent)
		parent->GetVirtualFunctionList(list);
	list.insert(list.end(), virtual_functions.begin(), virtual_functions.end());
}

size_t ScriptClassData::GetVirtualFunctionCount() const
{
	if (parent)
		return virtual_functions.size() + parent->GetVirtualFunctionCount();
	return virtual_functions.size();
}
