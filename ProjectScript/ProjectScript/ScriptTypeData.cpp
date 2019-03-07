#include "ScriptTypeData.h"

#include "ScriptClassData.h"

ScriptTypeData::ScriptTypeData()
{
	type = ST_VOID;
	indirection = 0;
	class_data = nullptr;
}

ScriptTypeData::~ScriptTypeData()
{
}

size_t ScriptTypeData::GetSize() const
{
	if (indirection != 0)
		return 8;
	if (type == ST_CLASS && class_data)
		return class_data->size;
	if (type == ST_VOID)
		return 0;
	return 8;
}

bool ScriptTypeData::operator==(const ScriptTypeData& rhs) const
{
	if (type != rhs.type)
		return false;
	if (indirection != rhs.indirection)
		return false;
	if (type == ST_CLASS)
		return class_data == rhs.class_data;
	return true;
}

bool ScriptTypeData::operator!=(const ScriptTypeData & rhs) const
{
	if (type != rhs.type)
		return true;
	if (indirection != rhs.indirection)
		return true;
	if (type == ST_CLASS)
		return class_data != rhs.class_data;
	return false;
}