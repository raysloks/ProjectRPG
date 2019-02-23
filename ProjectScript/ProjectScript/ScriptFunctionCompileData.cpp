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
	std::vector<uint8_t> free_regs = { 0b1001, 0b1000, 0b10 };
	std::vector<uint8_t> busy_regs;
	for (size_t i = 0; i < prototype.params.size(); ++i)
	{
		if (prototype.params[i].GetSize() <= 8 && !free_regs.empty())
		{
			comp.AddParameterRegister(parameter_names[i], prototype.params[i], free_regs.back());
			busy_regs.push_back(free_regs.back());
			free_regs.pop_back();
			continue;
		}
		comp.AddParameterStack(parameter_names[i], prototype.params[i], offset);
		offset += prototype.params[i].size;
	}

	comp.proto.reset(new ScriptFunctionPrototype(prototype));

	comp.BeginFunction();

	//if (prototype.cc == CC_THISCALL)
	{
		comp.SetBusy(0b001);
		for (auto reg : busy_regs)
			comp.SetBusy(reg);

		ScriptTypeData type_data;
		type_data.type = ST_CLASS;
		type_data.class_data = class_ptr;
		type_data.size = class_ptr->size;
		type_data.indirection = 1;

		ScriptCompileMemoryTarget target;
		target.mode = 0b11;
		target.regm = 0b001;

		ScriptVariableData variable_data;
		variable_data.type = type_data;
		variable_data.target = target;

		comp.scope.back().vars.insert(std::make_pair("this", variable_data));
	}

	code->compile(comp);

	//if (prototype.cc == CC_THISCALL)
	{
		comp.SetFree(0b001);
		for (auto reg : busy_regs)
			comp.SetFree(reg);
	}

	comp.EndFunction();

	comp.proto.reset();

	comp.EndScope();
}