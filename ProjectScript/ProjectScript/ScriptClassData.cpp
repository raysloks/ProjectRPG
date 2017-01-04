#include "ScriptClassData.h"

ScriptClassData::ScriptClassData()
{
}

ScriptClassData::~ScriptClassData()
{
}

void ScriptClassData::AddMember(const std::string& name, ScriptTypeData type)
{
	ScriptVariableData member_data;
	member_data.type = type;
	
	member_data.target.lvalue = false; // shouldn't really matter, should it?
	member_data.target.offset = size;
	member_data.target.mod = 0b10; // 0b01 for one byte signed offset, could be optimized elsewhere
	member_data.target.rm = 0b001;

	members.insert(std::make_pair(name, member_data));
	size += type.size;
	if (type.size % 4 != 0)
		size += 4 - type.size % 4;
}

void ScriptClassData::AddFunction(const std::string& name, ScriptFunctionPrototype prototype, void * pointer)
{
	functions.insert(std::make_pair(name, std::make_pair(prototype, pointer)));
}