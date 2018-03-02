#include "ScriptVariableMetaData.h"

ScriptVariableMetaData::ScriptVariableMetaData(ScriptVariableTypeData t, unsigned int o)
{
	type = t;
	offset = o;
}

ScriptVariableMetaData::~ScriptVariableMetaData()
{
}