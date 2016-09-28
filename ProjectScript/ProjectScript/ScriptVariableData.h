#ifndef SCRIPT_VARIABLE_DATA_H
#define SCRIPT_VARIABLE_DATA_H

#include "ScriptTypeData.h"

class ScriptVariableData
{
public:
	ScriptVariableData();
	~ScriptVariableData();

	size_t offset;
	ScriptTypeData type;
};

#endif