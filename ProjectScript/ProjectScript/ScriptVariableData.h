#ifndef SCRIPT_VARIABLE_DATA_H
#define SCRIPT_VARIABLE_DATA_H

#include "ScriptTypeData.h"
#include "ScriptCompileMemoryTarget.h"

class ScriptVariableData
{
public:
	ScriptVariableData();
	~ScriptVariableData();

	ScriptTypeData type;
	ScriptCompileMemoryTarget target;
};

#endif