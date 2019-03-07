#pragma once

#include <cstdint>

enum ScriptVariableType
{
	SVT_VOID,
	SVT_INTEGER_SIGNED,
	SVT_INTEGER_UNSIGNED,
	SVT_FLOAT,
	SVT_FUNCTION,
	SVT_CLASS
};

class ScriptBaseClassMetaData;

class ScriptTypeDataOld
{
public:
	ScriptTypeDataOld();
	ScriptTypeDataOld(ScriptVariableType type);

	ScriptVariableType type;
	const ScriptBaseClassMetaData * script_class;
};

ScriptTypeDataOld _svt_get(int i);
ScriptTypeDataOld _svt_get(unsigned int ui);
ScriptTypeDataOld _svt_get(uint64_t ui);
ScriptTypeDataOld _svt_get(float f);

template <typename T>
ScriptTypeDataOld _svt_get(T t)
{
	ScriptTypeDataOld ret(SVT_CLASS);
	ret.script_class = &T::meta;
	return ret;
}