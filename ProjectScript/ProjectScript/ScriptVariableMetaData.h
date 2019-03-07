#pragma once

#include "ScriptVariableTypeData.h"

class ScriptVariableMetaData
{
public:
	ScriptVariableMetaData(ScriptVariableTypeData type, unsigned int offset);
	~ScriptVariableMetaData();

	ScriptVariableTypeData type;
	unsigned int offset;
};

