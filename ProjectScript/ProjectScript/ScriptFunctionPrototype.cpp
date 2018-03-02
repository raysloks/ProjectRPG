#include "ScriptFunctionPrototype.h"

ScriptFunctionPrototype::ScriptFunctionPrototype()
{
}

ScriptFunctionPrototype::~ScriptFunctionPrototype()
{
}

uint32_t ScriptFunctionPrototype::getParamsSize() const
{
	uint32_t size = 0;
	for (auto param : params)
	{
		size += param.GetSize();
	}
	return size;
}

bool ScriptFunctionPrototype::operator==(const ScriptFunctionPrototype& rhs) const
{
	if (ret != rhs.ret)
		return false;
	if (cc != rhs.cc)
		return false;
	if (params.size() != rhs.params.size())
		return false;
	for (size_t i = 0; i < params.size(); i++)
	{
		if (params[i] != rhs.params[i])
			return false;
	}
	return true;
}

bool ScriptFunctionPrototype::operator!=(const ScriptFunctionPrototype& rhs) const
{
	if (ret != rhs.ret)
		return true;
	if (cc != rhs.cc)
		return true;
	if (params.size() != rhs.params.size())
		return true;
	for (size_t i = 0; i < params.size(); i++)
	{
		if (params[i] != rhs.params[i])
			return true;
	}
	return false;
}