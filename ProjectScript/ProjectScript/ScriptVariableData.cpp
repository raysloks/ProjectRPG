#include "ScriptVariableData.h"

ScriptVariableData::ScriptVariableData()
{
}

ScriptVariableData::~ScriptVariableData()
{
}

bool ScriptVariableData::operator==(const ScriptVariableData& rhs) const
{
	return type == rhs.type && target == rhs.target;
}

bool ScriptVariableData::operator!=(const ScriptVariableData& rhs) const
{
	return type != rhs.type || target != rhs.target;
}