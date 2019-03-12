#ifndef SCRIPT_FUNCTION_PROTOTYPE_H
#define SCRIPT_FUNCTION_PROTOTYPE_H

#include "ScriptTypeData.h"

#include <vector>

enum CallingConvention
{
	CC_CDECL,
	CC_STDCALL,
	CC_THISCALL,
	CC_MICROSOFT_X64
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

template <typename R, typename ...Args>
ScriptFunctionPrototype NewScriptFunctionPrototype()
{
	std::array<ScriptTypeData, sizeof...(Args)> args = { (NewScriptTypeData<Args>())... };

	ScriptFunctionPrototype prototype;
	prototype.params = std::vector<ScriptTypeData>(args.begin(), args.end());
	prototype.ret = NewScriptTypeData<R>();

	return prototype;
}

template <typename R, typename T, typename ...Args>
ScriptFunctionPrototype NewScriptFunctionPrototype(R(T::*f)(Args...))
{
	return NewScriptFunctionPrototype<R, Args...>();
}

#endif