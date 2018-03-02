#ifndef SCRIPT_FUNCTION_PROTOTYPE_H
#define SCRIPT_FUNCTION_PROTOTYPE_H

#include "ScriptTypeData.h"

#include <vector>

enum CallingConvention
{
	CC_CDECL,
	CC_STDCALL,
	CC_THISCALL
};

class ScriptFunctionPrototype
{
public:
	ScriptFunctionPrototype();
	~ScriptFunctionPrototype();

	uint32_t getParamsSize() const;

	std::vector<ScriptTypeData> params;
	ScriptTypeData ret;
	CallingConvention cc;

	bool operator==(const ScriptFunctionPrototype& rhs) const;
	bool operator!=(const ScriptFunctionPrototype& rhs) const;
};

#endif