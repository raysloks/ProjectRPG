#pragma once

enum ScriptVariableType
{
	VOID,
	INTEGER_SIGNED,
	INTEGER_UNSIGNED,
	FLOAT,
	FUNCTION,
	CLASS
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
ScriptTypeDataOld _svt_get(float f);

template <typename T>
ScriptTypeDataOld _svt_get(T t)
{
	ScriptTypeDataOld ret(CLASS);
	ret.script_class = &T::meta;
	return ret;
}