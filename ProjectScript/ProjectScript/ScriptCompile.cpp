#include "ScriptCompile.h"

#include "StreamAssignee.h"

ScriptCompile::ScriptCompile()
{
	scope.push_back(ScriptScope());

	StreamAssignee<uint8_t> p(ss);
	StreamAssignee<uint8_t> po(ss);
	StreamAssignee<uint8_t> o(ss);
	StreamAssignee<uint8_t> dat8(ss);
	StreamAssignee<uint16_t> dat16(ss);
	StreamAssignee<uint32_t> dat32(ss);
	StreamAssignee<uint64_t> dat64(ss);
	StreamAssigneeRelative<int8_t> rel8(*this);
	StreamAssigneeRelative<int32_t> rel32(*this);

	// push ebp
	po = 0x55;

	// mov ebp, esp
	po = 0x89;
	o = 0b11100101;

	stack = 0;
}

ScriptCompile::~ScriptCompile()
{
}

void ScriptCompile::BeginScope()
{
	size_t noffset = ss.str().size();
	scope.push_back(ScriptScope());
	scope.back().offset = noffset;
}

void ScriptCompile::EndScope()
{
	size_t block_size = ss.str().size() - scope.back().offset;

	size_t minoff = scope[scope.size() - 2].offset;
	size_t maxoff = scope.back().offset;

	for (auto i = rel8.begin(); i != rel8.end(); ++i)
	{
		if (*i >= minoff && *i < maxoff)
		{
			int8_t off = GetAt<int8_t>(*i);
			if (off + *i + sizeof(int8_t) >= scope.back().offset)
				SetAt<int8_t>(*i, off + block_size);
		}
	}
	for (auto i = rel32.begin(); i != rel32.end(); ++i)
	{
		if (*i >= minoff && *i < maxoff)
		{
			int32_t off = GetAt<int32_t>(*i);
			if (off + *i + sizeof(int32_t) >= scope.back().offset)
				SetAt<int32_t>(*i, off + block_size);
		}
	}

	size_t stack_dec = 0;
	for (auto i = scope.back().vars.begin(); i != scope.back().vars.end(); ++i)
	{
		stack_dec += i->second.type.size;
	}

	if (stack_dec > 0)
	{
		stack -= stack_dec;

		StreamAssignee<uint8_t> po(ss);
		StreamAssignee<uint8_t> o(ss);
		StreamAssignee<uint8_t> dat8(ss);
		StreamAssignee<uint32_t> dat32(ss);

		if (stack_dec < 128)
		{
			// add esp, stack_dec
			po = 0x83;
			o = 0b11000100;
			dat8 = stack_dec;
		}
		else
		{
			// add esp, stack_dec
			po = 0x81;
			o = 0b11000100;
			dat32 = stack_dec;
		}
	}

	scope.pop_back();
}

void ScriptCompile::Insert(size_t start, off_t size)
{
	for (auto i = rel8.begin(); i != rel8.end(); ++i)
	{
		if (*i >= start)
			*i += size;
	}
	for (auto i = rel32.begin(); i != rel32.end(); ++i)
	{
		if (*i >= start)
			*i += size;
	}
}

void ScriptCompile::PushVariable(const std::string & name, ScriptTypeData type)
{
	ScriptVariableData varData;
	varData.type = type;
	stack += type.size;
	varData.offset = stack;

	scope.back().vars[name] = varData;

	StreamAssignee<uint8_t> po(ss);
	StreamAssignee<uint8_t> o(ss);
	StreamAssignee<uint8_t> dat8(ss);
	StreamAssignee<uint32_t> dat32(ss);

	if (type.size < 128)
	{
		// sub esp, type.size
		po = 0x83;
		o = 0b11101100;
		dat8 = type.size;
	}
	else
	{
		// sub esp, type.size
		po = 0x81;
		o = 0b11101100;
		dat32 = type.size;
	}
}

void ScriptCompile::PushVariable(const std::string & name, ScriptTypeData type, ScriptCompileMemoryTarget target)
{
	ScriptVariableData varData;
	varData.type = type;
	stack += type.size;
	varData.offset = stack;

	scope.back().vars[name] = varData;

	StreamAssignee<uint8_t> po(ss);
	StreamAssignee<uint8_t> o(ss);
	StreamAssignee<uint8_t> dat8(ss);
	StreamAssignee<uint32_t> dat32(ss);

	if (target.mod == 0b11)
	{
		// push target
		po = 0x50 + target.rm;
	}
	else
	{
		// push target
		po = 0xff;
		o = target.GetModRegRM(6);
		if (target.mod == 0b01)
			dat8 = target.offset;
		if (target.mod == 0b10)
			dat32 = target.offset;
	}
}

void ScriptCompile::PushVariable(const std::string & name, ScriptTypeData type, uint32_t value)
{
	ScriptVariableData varData;
	varData.type = type;
	stack += type.size;
	varData.offset = stack;

	scope.back().vars[name] = varData;

	StreamAssignee<uint8_t> po(ss);
	StreamAssignee<uint32_t> dat32(ss);

	// push value
	po = 0x68;
	dat32 = value;
}

ScriptVariableData ScriptCompile::GetVariable(const std::string& name)
{
	for (auto i = scope.rbegin(); i != scope.rend(); ++i)
	{
		auto var = i->vars.find(name);
		if (var != i->vars.end())
			return var->second;
	}
	throw std::runtime_error("Couldn't find variable '" + name + "'.");
}
