#ifndef SCRIPT_TYPE_DATA_H
#define SCRIPT_TYPE_DATA_H

#include <memory>

enum ScriptType
{
	ST_VOID,
	ST_BOOL,
	ST_INT,
	ST_UINT,
	ST_FLOAT,
	ST_FUNCTION,
	ST_CLASS
};

class ScriptClassData;
class ScriptFunctionPrototype;

class ScriptTypeData
{
public:
	ScriptTypeData();
	~ScriptTypeData();

	ScriptType type;
	size_t indirection;
	std::shared_ptr<ScriptClassData> class_data;
	std::shared_ptr<ScriptFunctionPrototype> function_prototype;

	size_t GetSize() const;

	bool operator==(const ScriptTypeData& rhs) const;
	bool operator!=(const ScriptTypeData& rhs) const;
};

template <class T,
	typename std::enable_if_t<std::is_signed<T>::value && std::is_integral<T>::value>* = nullptr>
	ScriptTypeData NewScriptTypeData()
{
	ScriptTypeData ret;
	ret.type = ST_INT;
	return ret;
}

template <class T,
	typename std::enable_if_t<std::is_unsigned<T>::value && std::is_integral<T>::value>* = nullptr>
	ScriptTypeData NewScriptTypeData()
{
	ScriptTypeData ret;
	ret.type = ST_UINT;
	return ret;
}

template <class T,
	typename std::enable_if_t<std::is_pointer<T>::value>* = nullptr>
	ScriptTypeData NewScriptTypeData()
{
	ScriptTypeData ret = NewScriptTypeData<std::decay<T>::type>();
	++ret.indirection;
	return ret;
}

#endif