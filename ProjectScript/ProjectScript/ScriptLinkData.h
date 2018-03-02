#pragma once

#include <memory>

#include "ScriptFunctionPrototype.h"

class ScriptClassData;

class ScriptLinkData
{
public:
	ScriptLinkData();
	~ScriptLinkData();

	bool relative;
	size_t location;
	std::shared_ptr<ScriptClassData> class_ptr;
	std::string function_name;
	ScriptFunctionPrototype prototype;
};