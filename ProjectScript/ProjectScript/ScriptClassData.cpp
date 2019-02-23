#include "ScriptClassData.h"

ScriptClassData::ScriptClassData()
{
	size = 0;
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
	if (members.find("_vfptr") == members.end())
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
	member_data.target.regm = 0b001; // rcx

	members.insert(std::make_pair(name, member_data));
	size_t member_size = type.GetSize();
	size += member_size;
	if (member_size % 8 != 0)
		size += 8 - member_size % 8;
}

void ScriptClassData::AddFunction(const std::string& name, const ScriptFunctionPrototype& prototype, void * pointer)
{
	if (parent)
	{
		if (parent->GetVirtualFunctionIndex(name) >= 0)
		{
			functions.insert(std::make_pair(class_name + "::" + name, std::make_pair(prototype, pointer)));
			return;
		}
	}
	functions.insert(std::make_pair(name, std::make_pair(prototype, pointer)));
}

void ScriptClassData::AddVirtualFunction(const std::string& name, const ScriptFunctionPrototype& prototype)
{
	if (members.find("_vfptr") == members.end())
		throw std::runtime_error("Cannot find virtual function table.");
	ScriptVirtualFunctionData func;
	func.name = name;
	func.offset = GetVirtualFunctionCount() * 8;
	func.prototype = prototype;
	virtual_functions.push_back(func);
}

ScriptVariableData ScriptClassData::GetMember(const std::string& name)
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

void * ScriptClassData::GetFunctionFinalAddress(const std::string& name, const ScriptFunctionPrototype& prototype)
{
	for (auto& func : functions)
	{
		if (func.first.compare(name) == 0 && func.second.first == prototype)
		{
			return func.second.second;
		}
	}
	if (parent)
		return parent->GetFunctionFinalAddress(name, prototype);
	return nullptr;
}

off_t ScriptClassData::GetVirtualFunctionIndex(const std::string& name)
{
	for (size_t i = 0; i < virtual_functions.size(); i++)
	{
		auto& func = virtual_functions[i];
		if (func.name.compare(name) == 0)
		{
			if (parent)
				return i + parent->GetVirtualFunctionCount();
			return i;
		}
	}
	if (parent)
		return parent->GetVirtualFunctionIndex(name);
	return -1;
}

size_t ScriptClassData::GetVirtualFunctionCount()
{
	if (parent)
		return virtual_functions.size() + parent->GetVirtualFunctionCount();
	return virtual_functions.size();
}
