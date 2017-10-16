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

	bool eax = comp.IsBusy(0b000);
	bool ecx = comp.IsBusy(0b001);
	bool edx = comp.IsBusy(0b010);
	if (eax && ecx && edx)
	{
		throw std::runtime_error("Unable to find free register.");
	}
	if (!edx)
		ret.rm = 0b010;
	if (!ecx)
		ret.rm = 0b001;
	if (!eax)
		ret.rm = 0b000;
	return ret;
}

ScriptCompileMemoryTarget ScriptAssemblyHelper::FindRegister(ScriptCompileMemoryTarget& target)
{
	comp.SetBusy(target.rm);
	auto ret = FindRegister();
	comp.SetFree(target.rm);
	return ret;
}

ScriptCompileMemoryTarget ScriptAssemblyHelper::FindRegister(const std::vector<ScriptCompileMemoryTarget>& targets)
{
	for each (auto target in targets)
		comp.SetBusy(target.rm);
	auto ret = FindRegister();
	for each (auto target in targets)
		comp.SetFree(target.rm);
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

	bool d_bit = opcode & 0b00000010;

	if (d_bit && destination.mod != 0b11)
	{
		throw std::runtime_error("Destination is not a register.");
	}

	if (!d_bit && source.mod != 0b11)
	{
		throw std::runtime_error("Source is not a register.");
	}

	po = opcode;
	if (d_bit)
	{
		o = source.GetModRegRM(destination.rm);
		if (source.mod == 0b01)
			dat8 = source.offset;
		if (source.mod == 0b10)
			dat32 = source.offset;
	}
	else
	{
		o = destination.GetModRegRM(source.rm);
		if (destination.mod == 0b01)
			dat8 = destination.offset;
		if (destination.mod == 0b10)
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
	if (operand.mod == 0b01)
		dat8 = operand.offset;
	if (operand.mod == 0b10)
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
	if (source.mod == 0b11)
	{
		po = 0x50 + source.rm;
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

	if (destination.mod == 0b11)
	{
		po = 0x58 + destination.rm;
	}
}
