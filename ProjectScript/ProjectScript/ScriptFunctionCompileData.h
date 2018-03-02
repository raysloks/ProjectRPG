#pragma once

#include <memory>

#include "ScriptClassData.h"

class ScriptCompile;
class Statement;

class ScriptFunctionCompileData
{
public:
	ScriptFunctionCompileData();
	~ScriptFunctionCompileData();

	void compile(ScriptCompile& comp);

	std::shared_ptr<ScriptClassData> class_ptr;
	std::string name;
	ScriptFunctionPrototype prototype;
	std::vector<std::string> parameter_names;
	std::shared_ptr<Statement> code;
};