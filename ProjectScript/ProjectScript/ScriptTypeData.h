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
	size_t size, indirection;
	std::shared_ptr<ScriptClassData> class_data;
	std::shared_ptr<ScriptFunctionPrototype> function_prototype;

	size_t GetSize() const;

	bool operator==(const ScriptTypeData& rhs) const;
	bool operator!=(const ScriptTypeData& rhs) const;
};

#endif