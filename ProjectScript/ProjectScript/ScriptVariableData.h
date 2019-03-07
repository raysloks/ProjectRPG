#pragma once

#include "ScriptTypeData.h"
#include "ScriptCompileMemoryTarget.h"

class ScriptVariableData
{
public:
	ScriptVariableData();
	~ScriptVariableData();

	ScriptTypeData type;
	ScriptCompileMemoryTarget target;

	bool operator==(const ScriptVariableData& rhs) const;
	bool operator!=(const ScriptVariableData& rhs) const;
};