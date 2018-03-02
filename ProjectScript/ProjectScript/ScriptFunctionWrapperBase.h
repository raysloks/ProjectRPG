#pragma once

#include "ScriptVariableTypeData.h"
#include "ScriptFunctionPrototypeOld.h"

#include <vector>

class ScriptFunctionWrapperBase
{
public:
	virtual ~ScriptFunctionWrapperBase() {};

	virtual void Call(void * instance, void * data) {}

	ScriptVariableTypeData GetReturnType()
	{
		return prototype.ret;
	}

	std::vector<ScriptVariableTypeData> GetArgumentTypes()
	{
		return prototype.args;
	}

	ScriptFunctionPrototypeOld prototype;
};

