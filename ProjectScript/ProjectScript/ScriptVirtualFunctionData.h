#pragma once

#include "ScriptFunctionPrototype.h"

class ScriptVirtualFunctionData
{
public:
	ScriptVirtualFunctionData();
	~ScriptVirtualFunctionData();

	std::string name;
	size_t offset; // isn't really used rn?
	ScriptFunctionPrototype prototype;
};