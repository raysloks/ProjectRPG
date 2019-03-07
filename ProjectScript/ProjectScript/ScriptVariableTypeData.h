#pragma once

#include "ScriptVariableTypes.h"

#include <type_traits>
#include <vector>

class ScriptBaseClassMetaData;

class ScriptVariableTypeData
{
public:
	ScriptVariableTypeData();
	~ScriptVariableTypeData();

	unsigned int indirection;
	ScriptTypeDataOld type;
	std::vector<ScriptVariableTypeData> parameters;
};

template <typename T>
ScriptVariableTypeData NewScriptTypeData(T * t)
{
	ScriptVariableTypeData ret = NewScriptTypeData(*t);
	++ret.indirection;
	return ret;
}

template <typename T>
ScriptVariableTypeData NewScriptTypeData(T t)
{
	ScriptVariableTypeData ret;
	ret.type = _svt_get(t);
	return ret;
}

