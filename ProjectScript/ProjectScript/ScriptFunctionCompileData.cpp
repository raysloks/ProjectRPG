#include "ScriptFunctionCompileData.h"

#include "ScriptCompile.h"
#include "Statement.h"

ScriptFunctionCompileData::ScriptFunctionCompileData()
{
}

ScriptFunctionCompileData::~ScriptFunctionCompileData()
{
}

void ScriptFunctionCompileData::compile(ScriptCompile& comp)
{
	comp.current_class = class_ptr;

	for (auto& func : class_ptr->functions)
	{
		if (func.first.compare(name) == 0 && func.second.first == prototype)
		{
			func.second.second = (char*)comp.base_pointer + comp.ss.tellp();
		}
	}

	comp.BeginScope();

	size_t offset = 8;
	for (size_t i = 0; i < prototype.params.size(); ++i)
	{
		comp.AddParameter(parameter_names[i], prototype.params[i], offset);
		offset += prototype.params[i].size;
	}

	comp.proto.reset(new ScriptFunctionPrototype(prototype));

	comp.BeginFunction();

	if (prototype.cc == CC_THISCALL)
	{
		comp.SetBusy(0b001);

		ScriptTypeData type_data;
		type_data.type = ST_CLASS;
		type_data.class_data = class_ptr;
		type_data.size = class_ptr->size;
		type_data.indirection = 1;

		ScriptCompileMemoryTarget target;
		target.mod = 0b11;
		target.rm = 0b001;

		ScriptVariableData variable_data;
		variable_data.type = type_data;
		variable_data.target = target;

		comp.scope.back().vars.insert(std::make_pair("this", variable_data));
	}

	code->compile(comp);

	if (prototype.cc == CC_THISCALL)
	{
		comp.SetFree(0b001);
	}

	comp.EndScope();

	comp.proto.reset();
}