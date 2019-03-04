#include "ScriptFunctionCompileData.h"

#include <algorithm>

#include "ScriptCompile.h"
#include "Statement.h"
#include "StreamAssignee.h"

ScriptFunctionCompileData::ScriptFunctionCompileData()
{
}

ScriptFunctionCompileData::~ScriptFunctionCompileData()
{
}

void ScriptFunctionCompileData::compile(ScriptCompile& comp)
{
	auto& sasm = comp.sasm;

	StreamAssignee<uint8_t> p(comp);
	StreamAssignee<uint8_t> po(comp);
	StreamAssignee<uint8_t> o(comp);
	StreamAssignee<uint8_t> dat8(comp);
	StreamAssignee<uint16_t> dat16(comp);
	StreamAssignee<uint32_t> dat32(comp);
	StreamAssignee<uint64_t> dat64(comp);

	comp.current_class = class_ptr;

	for (auto& func : class_ptr->functions)
	{
		if (func.first.compare(class_ptr->class_name + "::" + name) == 0 && func.second.first == prototype)
		{
			func.second.second = (char*)comp.base_pointer + comp.ss.tellp();
		}
	}

	comp.BeginScope();

	sasm.Move(0x89, ScriptCompileMemoryTarget(0b01, 0b100, 8), ScriptCompileMemoryTarget(0b011));
	sasm.Move(0x89, ScriptCompileMemoryTarget(0b011), ScriptCompileMemoryTarget(0b001));

	std::vector<uint8_t> free_regs = { 0b1001, 0b1000, 0b10 };
	std::vector<uint8_t> busy_regs;
	for (size_t i = 0; i < prototype.params.size(); ++i)
	{
		comp.AddParameterStack(parameter_names[i], prototype.params[i], i * 8 + 8 + 8);
		if (!free_regs.empty())
		{
			//comp.AddParameterRegister(parameter_names[i], prototype.params[i], free_regs.back());
			//busy_regs.push_back(free_regs.back());
			sasm.Move(0x89, ScriptCompileMemoryTarget(0b01, 0b100, i * 8 + 8 + 8), ScriptCompileMemoryTarget(free_regs.back()));
			free_regs.pop_back();
		}
		else
		{
			//comp.AddParameterStack(parameter_names[i], prototype.params[i], i * 8 + 8 + 8);
		}
	}

	comp.proto.reset(new ScriptFunctionPrototype(prototype));

	comp.BeginFunction();

	//if (prototype.cc == CC_THISCALL)
	{
		//comp.SetBusy(0b001);
		for (auto reg : busy_regs)
			comp.SetBusy(reg);

		ScriptTypeData type_data;
		type_data.type = ST_CLASS;
		type_data.class_data = class_ptr;
		type_data.size = class_ptr->size;
		type_data.indirection = 1;

		ScriptCompileMemoryTarget target;
		target.mode = 0b11;
		target.regm = 0b011;

		ScriptVariableData variable_data;
		variable_data.type = type_data;
		variable_data.target = target;

		comp.scope.back().vars.insert(std::make_pair("this", variable_data));
	}

	if (name.compare(class_ptr->class_name) == 0)
	{
		if (class_ptr->vftable)
		{
			auto tmp_target = sasm.FindRegister();
			uint8_t rex = 0b01001000;
			if (tmp_target.regm & 0b1000)
				rex |= 0b001;
			p = rex;
			po = 0xb8 + (tmp_target.regm & 0b111);
			dat64 = (uint64_t)class_ptr->vftable;

			sasm.Move(0x89, class_ptr->GetMember("_vfptr").target, tmp_target);
		}
	}

	code->compile(comp);

	//if (prototype.cc == CC_THISCALL)
	{
		for (auto reg : busy_regs)
			comp.SetFree(reg);
		//comp.SetFree(0b001);
	}

	if (name.compare("~") == 0)
	{
		std::vector<ScriptVariableData> members;
		class_ptr->GetMemberList(members);
		std::sort(members.begin(), members.end(), [](auto a, auto b)
		{
			return a.target.offset > b.target.offset;
		});
		int32_t offset = 0;
		for (auto member : members)
		{
			if (member.type.indirection == 0 && member.type.type == ST_CLASS)
			{
				auto function_pointer = member.type.class_data->GetFunctionFinalAddress("~", ScriptFunctionPrototype());
				if (function_pointer)
				{
					int32_t diff = member.target.offset - offset;

					if (diff >= -128 && diff <= 127)
					{
						// add rcx, diff
						p = 0b01001000;
						po = 0x83;
						o = 0b11000001;
						dat8 = diff;
					}
					else
					{
						// add rcx, diff
						p = 0b01001000;
						po = 0x81;
						o = 0b11000001;
						dat32 = diff;
					}

					sasm.Push(ScriptCompileMemoryTarget(0b001));

					// call
					po = 0xe8;

					ScriptLinkData link;
					link.relative = true;
					link.location = comp.ss.tellp();
					link.class_ptr = member.type.class_data;
					link.function_name = "~";
					link.prototype = ScriptFunctionPrototype();
					comp.links.push_back(link);

					dat32 = (char*)function_pointer - ((char*)comp.base_pointer + comp.ss.tellp() + 4);

					sasm.Pop(ScriptCompileMemoryTarget(0b001));

					offset = member.target.offset;
				}
			}
		}
	}

	comp.EndFunction();

	comp.proto.reset();

	comp.EndScope();
}