#include "ScriptAssemblyHelper.h"

#include "StreamAssignee.h"
#include "ScriptCompile.h"

ScriptAssemblyHelper::ScriptAssemblyHelper(ScriptCompile& c) : comp(c), ss(comp.ss)
{
}

ScriptAssemblyHelper::~ScriptAssemblyHelper()
{
}

ScriptCompileMemoryTarget ScriptAssemblyHelper::FindRegister()
{
	ScriptCompileMemoryTarget ret;

	std::vector<uint8_t> regs = { 0b000, 0b001, 0b010, 8, 9, 10, 11 };

	for (auto reg : regs)
	{
		if (comp.IsFree(reg))
		{
			ret.regm = reg;
			return ret;
		}
	}

	throw std::runtime_error("Unable to find free register.");
}

ScriptCompileMemoryTarget ScriptAssemblyHelper::FindRegister(ScriptCompileMemoryTarget& target)
{
	if (comp.IsFree(target.regm))
	{
		comp.SetBusy(target.regm);
		auto ret = FindRegister();
		comp.SetFree(target.regm);
		return ret;
	}
	else
	{
		return FindRegister();
	}
}

ScriptCompileMemoryTarget ScriptAssemblyHelper::FindRegister(const std::vector<ScriptCompileMemoryTarget>& targets)
{
	std::vector<uint8_t> free_targets;
	for each (auto target in targets)
		if (comp.IsFree(target.regm))
			free_targets.push_back(target.regm);
	for each (auto target in free_targets)
		comp.SetBusy(target);
	auto ret = FindRegister();
	for each (auto target in free_targets)
		comp.SetFree(target);
	return ret;
}

void ScriptAssemblyHelper::Move(uint8_t opcode, ScriptCompileMemoryTarget& destination, ScriptCompileMemoryTarget& source)
{
	StreamAssignee<uint8_t> p(comp);
	StreamAssignee<uint8_t> po(comp);
	StreamAssignee<uint8_t> o(comp);
	StreamAssignee<uint8_t> dat8(comp);
	StreamAssignee<uint16_t> dat16(comp);
	StreamAssignee<uint32_t> dat32(comp);
	StreamAssignee<uint64_t> dat64(comp);

	if (destination.mode != 0b11 && source.mode != 0b11)
	{
		throw std::runtime_error("Cannot operate from memory to memory.");
	}

	bool d_bit = destination.mode == 0b11;

	opcode &= 0b11111101;
	if (d_bit)
		opcode |= 0b00000010;

	uint8_t rex = 0b01001000;

	if (d_bit)
	{
		if (destination.regm > 7)
			rex |= 0b100;
		if (source.regm > 7)
			rex |= 0b001;

		p = rex;
		po = opcode;
		o = source.GetModRegRM(destination.regm);
		if (source.mode == 0b01)
			dat8 = source.offset;
		if (source.mode == 0b10)
			dat32 = source.offset;
	}
	else
	{
		if (source.regm > 7)
			rex |= 0b100;
		if (destination.regm > 7)
			rex |= 0b001;

		p = rex;
		po = opcode;
		o = destination.GetModRegRM(source.regm);
		if (destination.mode == 0b01)
			dat8 = destination.offset;
		if (destination.mode == 0b10)
			dat32 = destination.offset;
	}
}

void ScriptAssemblyHelper::Move(uint8_t opcode, uint8_t opcode_extension, ScriptCompileMemoryTarget& operand)
{
	StreamAssignee<uint8_t> p(comp);
	StreamAssignee<uint8_t> po(comp);
	StreamAssignee<uint8_t> o(comp);
	StreamAssignee<uint8_t> dat8(comp);
	StreamAssignee<uint16_t> dat16(comp);
	StreamAssignee<uint32_t> dat32(comp);
	StreamAssignee<uint64_t> dat64(comp);

	po = opcode;
	o = operand.GetModRegRM(opcode_extension);
	if (operand.mode == 0b01)
		dat8 = operand.offset;
	if (operand.mode == 0b10)
		dat32 = operand.offset;
}

void ScriptAssemblyHelper::Push(ScriptCompileMemoryTarget& source)
{
	StreamAssignee<uint8_t> p(comp);
	StreamAssignee<uint8_t> po(comp);
	StreamAssignee<uint8_t> o(comp);
	StreamAssignee<uint8_t> dat8(comp);
	StreamAssignee<uint16_t> dat16(comp);
	StreamAssignee<uint32_t> dat32(comp);
	StreamAssignee<uint64_t> dat64(comp);

	/*if (source.immediate)
	{
		po = 0x68;
		dat32 = source.offset;
		return;
	}*/
	if (source.mode == 0b11)
	{
		po = 0x50 + source.regm;
	}
}

void ScriptAssemblyHelper::Pop(ScriptCompileMemoryTarget& destination)
{
	StreamAssignee<uint8_t> p(comp);
	StreamAssignee<uint8_t> po(comp);
	StreamAssignee<uint8_t> o(comp);
	StreamAssignee<uint8_t> dat8(comp);
	StreamAssignee<uint16_t> dat16(comp);
	StreamAssignee<uint32_t> dat32(comp);
	StreamAssignee<uint64_t> dat64(comp);

	if (destination.mode == 0b11)
	{
		po = 0x58 + destination.regm;
	}
}
