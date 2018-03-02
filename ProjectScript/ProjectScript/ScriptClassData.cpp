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

void ScriptClassData::AddMember(const std::string& name, const ScriptTypeData& type)
{
	ScriptVariableData member_data;
	member_data.type = type;
	
	member_data.target.lvalue = false; // shouldn't really matter, should it?
	member_data.target.offset = size;
	member_data.target.mod = 0b10; // 0b01 for one byte signed offset, could be optimized elsewhere
	member_data.target.rm = 0b001; // ecx

	members.insert(std::make_pair(name, member_data));
	size_t member_size = type.GetSize();
	size += member_size;
	if (member_size % 4 != 0)
		size += 4 - member_size % 4;
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
	size_t count = GetVirtualFunctionCount();
	if (count == 0)
	{
		for (auto& member : members)
		{
			member.second.target.offset += 4;
		}

		ScriptVariableData member_data;
		member_data.type.type = ST_VOID;
		member_data.type.indirection = 2;

		member_data.target.lvalue = false; // shouldn't really matter, should it?
		if (parent)
			member_data.target.offset = parent->size;
		else
			member_data.target.offset = 0;
		member_data.target.mod = 0b10; // 0b01 for one byte signed offset, could be optimized elsewhere
		member_data.target.rm = 0b001; // ecx

		members.insert(std::make_pair("vftable", member_data));

		size += 4;
	}

	ScriptVirtualFunctionData func;
	func.name = name;
	func.offset = count * 4;
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
