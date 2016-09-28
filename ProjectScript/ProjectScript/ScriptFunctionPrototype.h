#ifndef SCRIPT_FUNCTION_PROTOTYPE_H
#define SCRIPT_FUNCTION_PROTOTYPE_H

#include "ScriptTypeData.h"

#include <vector>

class ScriptFunctionPrototype
{
public:
	ScriptFunctionPrototype();
	~ScriptFunctionPrototype();

	std::vector<ScriptTypeData> params;
	ScriptTypeData ret;
};

#endif