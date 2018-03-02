#include "ScriptTypeData.h"

ScriptTypeData::ScriptTypeData()
{
	type = ST_VOID;
	size = 0;
	indirection = 0;
}

ScriptTypeData::~ScriptTypeData()
{
}

size_t ScriptTypeData::GetSize() const
{
	if (indirection != 0)
		return 4;
	return size;
}

bool ScriptTypeData::operator==(const ScriptTypeData & rhs) const
{
	return size == rhs.size && type == rhs.type;
}

bool ScriptTypeData::operator!=(const ScriptTypeData & rhs) const
{
	return size != rhs.size || type != rhs.type;
}