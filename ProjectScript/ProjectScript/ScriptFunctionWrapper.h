#pragma once

#include "ScriptFunctionWrapperBase.h"

#include <tuple>
#include <functional>
#include <array>
#include <utility>

template <typename T>
class ScriptFunctionWrapper;

template <typename R, typename T, typename ...Args>
class ScriptFunctionWrapper<R(T::*)(Args...)> : public ScriptFunctionWrapperBase
{
public:
	typedef R(T::*FuncType)(Args...);

	ScriptFunctionWrapper(FuncType f)
	{
		func = f;
		prototype = NewScriptFunctionPrototype<R, Args...>();
	}
	~ScriptFunctionWrapper() {}

	template <size_t ...I>
	void DeepCall(void * instance, void * data, std::index_sequence<I...>)
	{
		*reinterpret_cast<R*>(data) = std::invoke(func, *reinterpret_cast<T*>(instance), (*reinterpret_cast<Args*>(((char*)data) +
			prototype.offsets[I]))...);
	}

	void Call(void * instance, void * data)
	{
		DeepCall(instance, data, std::make_index_sequence<sizeof...(Args)>());
	}

	FuncType func;
};

template <typename T>
ScriptFunctionWrapperBase * NewScriptFunctionWrapper(T f)
{
	return new ScriptFunctionWrapper<T>(f);
}
