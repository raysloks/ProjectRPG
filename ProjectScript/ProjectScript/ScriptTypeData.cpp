#include "ScriptTypeData.h"

ScriptTypeData::ScriptTypeData()
{
	size = 0;
	type = ST_VOID;
}

ScriptTypeData::~ScriptTypeData()
{
}

bool ScriptTypeData::operator==(const ScriptTypeData & rhs) const
{
	return size == rhs.size && type == rhs.type;
}

bool ScriptTypeData::operator!=(const ScriptTypeData & rhs) const
{
	return size != rhs.size || type != rhs.type;
}
