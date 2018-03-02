#pragma once

#include <vector>

#include "ScriptVariableTypeData.h"

class ScriptFunctionPrototypeOld
{
public:
	ScriptVariableTypeData ret;
	std::vector<size_t> offsets;
	std::vector<ScriptVariableTypeData> args;
};

template <typename A>
size_t _getOffset(size_t& args_size)
{
	size_t prev = args_size;
	args_size += sizeof(A);
	return prev;
}

template <typename A>
ScriptVariableTypeData _getType()
{
	A a = A();
	ScriptVariableTypeData type = NewScriptTypeData(a);
	return type;
}

template <typename R, typename ...Args>
ScriptFunctionPrototypeOld NewScriptFunctionPrototype()
{
	size_t args_size = 0;
	std::array<size_t, sizeof...(Args)> offsets = { (_getOffset<Args>(args_size))... };
	std::array<ScriptVariableTypeData, sizeof...(Args)> args = { (_getType<Args>())... };

	ScriptFunctionPrototypeOld prototype;
	prototype.offsets = std::vector<size_t>(offsets.begin(), offsets.end());
	prototype.args = std::vector<ScriptVariableTypeData>(args.begin(), args.end());
	prototype.ret = _getType<R>();

	return prototype;
}

template <typename R, typename T, typename ...Args>
ScriptFunctionPrototypeOld NewScriptFunctionPrototype(R(T::*f)(Args...))
{
	return NewScriptFunctionPrototype<R, Args...>();
}