#include "ScriptAssemblyHelper.h"

#include "StreamAssignee.h"

ScriptAssemblyHelper::ScriptAssemblyHelper(ScriptCompile& comp) : ss(comp.ss)
{
}

ScriptAssemblyHelper::~ScriptAssemblyHelper()
{
}

ScriptCompileMemoryTarget ScriptAssemblyHelper::FindRegister(const std::vector<ScriptCompileMemoryTarget>& targets)
{
	ScriptCompileMemoryTarget ret;

	bool eax = false;
	bool ebx = false;
	bool edx = false;
	bool esi = false;
	bool edi = false;
	for (auto i = targets.begin(); i != targets.end(); ++i)
	{
		eax |= i->rm == 0b000;
		ebx |= i->rm == 0b011;
		edx |= i->rm == 0b010;
		esi |= i->rm == 0b110;
		edi |= i->rm == 0b111;
	}
	if (eax && ebx && edx && esi && edi)
	{
		throw std::runtime_error("Unable to find free register.");
	}
	if (!edi)
		ret.rm = 0b111;
	if (!esi)
		ret.rm = 0b110;
	if (!edx)
		ret.rm = 0b010;
	if (!ebx)
		ret.rm = 0b011;
	if (!eax)
		ret.rm = 0b000;
	return ret;
}

void ScriptAssemblyHelper::Move(uint8_t opcode, ScriptCompileMemoryTarget & destination, ScriptCompileMemoryTarget & source)
{
	StreamAssignee<uint8_t> p(ss);
	StreamAssignee<uint8_t> po(ss);
	StreamAssignee<uint8_t> o(ss);
	StreamAssignee<uint8_t> dat8(ss);
	StreamAssignee<uint16_t> dat16(ss);
	StreamAssignee<uint32_t> dat32(ss);
	StreamAssignee<uint64_t> dat64(ss);

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

void ScriptAssemblyHelper::Move(uint8_t opcode, uint8_t opcode_extension, ScriptCompileMemoryTarget & operand)
{
	StreamAssignee<uint8_t> p(ss);
	StreamAssignee<uint8_t> po(ss);
	StreamAssignee<uint8_t> o(ss);
	StreamAssignee<uint8_t> dat8(ss);
	StreamAssignee<uint16_t> dat16(ss);
	StreamAssignee<uint32_t> dat32(ss);
	StreamAssignee<uint64_t> dat64(ss);

	po = opcode;
	o = operand.GetModRegRM(opcode_extension);
	if (operand.mod == 0b01)
		dat8 = operand.offset;
	if (operand.mod == 0b10)
		dat32 = operand.offset;
}

void ScriptAssemblyHelper::Push(ScriptCompileMemoryTarget & source)
{
	StreamAssignee<uint8_t> p(ss);
	StreamAssignee<uint8_t> po(ss);
	StreamAssignee<uint8_t> o(ss);
	StreamAssignee<uint8_t> dat8(ss);
	StreamAssignee<uint16_t> dat16(ss);
	StreamAssignee<uint32_t> dat32(ss);
	StreamAssignee<uint64_t> dat64(ss);

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

void ScriptAssemblyHelper::Pop(ScriptCompileMemoryTarget & destination)
{
	StreamAssignee<uint8_t> p(ss);
	StreamAssignee<uint8_t> po(ss);
	StreamAssignee<uint8_t> o(ss);
	StreamAssignee<uint8_t> dat8(ss);
	StreamAssignee<uint16_t> dat16(ss);
	StreamAssignee<uint32_t> dat32(ss);
	StreamAssignee<uint64_t> dat64(ss);

	if (destination.mod == 0b11)
	{
		po = 0x58 + destination.rm;
	}
}
