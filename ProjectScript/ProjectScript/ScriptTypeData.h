#ifndef SCRIPT_TYPE_DATA_H
#define SCRIPT_TYPE_DATA_H

enum ScriptType
{
	ST_VOID,
	ST_BOOL,
	ST_INT,
	ST_UINT,
	ST_FLOAT,
	ST_FUNCTION,
	ST_POINTER,
	ST_CLASS
};

class ScriptTypeData
{
public:
	ScriptTypeData();
	~ScriptTypeData();

	ScriptType type;
	size_t size;

	bool operator==(const ScriptTypeData& rhs) const;
	bool operator!=(const ScriptTypeData& rhs) const;
};

#endif