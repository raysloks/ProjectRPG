#include "ScriptCompile.h"

#include <algorithm>

#include "StreamAssignee.h"

ScriptCompile::ScriptCompile(void * bp) : sasm(*this)
{
	base_pointer = bp;
}

ScriptCompile::~ScriptCompile()
{
}

void ScriptCompile::BeginFunction()
{
	StreamAssignee<uint8_t> p(*this);
	StreamAssignee<uint8_t> po(*this);
	StreamAssignee<uint8_t> o(*this);
	StreamAssignee<uint8_t> dat8(*this);
	StreamAssignee<uint16_t> dat16(*this);
	StreamAssignee<uint32_t> dat32(*this);
	StreamAssignee<uint64_t> dat64(*this);
	StreamAssigneeRelative<int8_t> rel8(*this);
	StreamAssigneeRelative<int32_t> rel32(*this);

	// push rbp
	po = 0x55;

	// mov rbp, rsp
	p = 0b01001000;
	po = 0x89;
	o = 0b11100101;

	// sub rsp, 0
	p = 0b01001000;
	po = 0x81;
	o = 0b11101100;
	stack_allocation = ss.str().size();
	dat32 = 0;

	max_stack = 0;
	stack = 0;
}

void ScriptCompile::EndFunction()
{
	SetAt<int32_t>(stack_allocation, max_stack);
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

	size_t minoff = 0;
	if (scope.size() >= 2)
		minoff = scope[scope.size() - 2].offset;
	size_t maxoff = scope.back().offset;

	for (auto i = rel8.begin(); i != rel8.end(); ++i)
	{
		if (*i >= minoff && *i < maxoff)
		{
			int8_t off = GetAt<int8_t>(*i);
			if (off + *i + sizeof(off) >= scope.back().offset)
				SetAt<int8_t>(*i, off + block_size);
		}
	}
	for (auto i = rel32.begin(); i != rel32.end(); ++i)
	{
		if (*i >= minoff && *i < maxoff)
		{
			int32_t off = GetAt<int32_t>(*i);
			if (off + *i + sizeof(off) >= scope.back().offset)
				SetAt<int32_t>(*i, off + block_size);
		}
	}

	size_t stack_dec = 0;
	for (auto i = scope.back().vars.begin(); i != scope.back().vars.end(); ++i)
	{
		if (i->second.target.regm == 0b101)
			if (i->second.target.offset < 0)
				stack_dec += i->second.type.size;
	}

	if (stack_dec > 0)
	{
		max_stack = std::max(max_stack, stack);
		stack -= stack_dec;

		//StreamAssignee<uint8_t> po(*this);
		//StreamAssignee<uint8_t> o(*this);
		//StreamAssignee<uint8_t> dat8(*this);
		//StreamAssignee<uint32_t> dat32(*this);

		//if (stack_dec < 256)
		//{
		//	// add esp, stack_dec
		//	po = 0x83;
		//	o = 0b11000100;
		//	dat8 = stack_dec;
		//}
		//else
		//{
		//	// add esp, stack_dec
		//	po = 0x81;
		//	o = 0b11000100;
		//	dat32 = stack_dec;
		//}
	}

	scope.pop_back();
}

void ScriptCompile::Compile()
{
	for (auto& func : function_code)
	{
		func.compile(*this);
	}
}

void ScriptCompile::Link()
{
	for (auto& link : links)
	{
		if (link.relative)
		{
			SetAt<int32_t>(link.location, (char*)link.class_ptr->GetFunctionFinalAddress(link.function_name, link.prototype) - ((char*)base_pointer + link.location + sizeof(int32_t)));
		}
		else
		{
			SetAt<uint64_t>(link.location, (uint64_t)link.class_ptr->GetFunctionFinalAddress(link.function_name, link.prototype));
		}
	}
}

void ScriptCompile::Cut(size_t start, size_t size)
{
	size_t end = ss.tellp();

	Adjust(start, -size);

	ss.seekg(start + size);
	ss.seekp(start);
	while (!ss.eof())
	{
		ss.put(ss.get());
	}
	ss.clear();
	ss.seekg(0);
	ss.seekp(end - size);
}

void ScriptCompile::Adjust(size_t start, off_t size)
{
	for (auto i = rel8.begin(); i != rel8.end(); ++i) // TODO check for overflow/underflow
	{
		int8_t off = GetAt<int8_t>(*i);
		if (*i >= start)
		{
			if (*i + sizeof(off) + off < start)
				SetAt<int8_t>(*i, off - size);
			*i += size;
		}
		else
		{
			if (*i + sizeof(off) + off >= start)
				SetAt<int8_t>(*i, off + size);
		}
	}
	for (auto i = rel32.begin(); i != rel32.end(); ++i)
	{
		int32_t off = GetAt<int32_t>(*i);
		if (*i >= start)
		{
			if (*i + sizeof(off) + off < start)
				SetAt<int32_t>(*i, off - size);
			*i += size;
		}
		else
		{
			if (*i + sizeof(off) + off >= start)
				SetAt<int32_t>(*i, off + size);
		}
	}
	for (auto i = links.begin(); i != links.end(); ++i)
	{
		if (i->location >= start)
		{
			i->location += size;
		}
	}
}

void ScriptCompile::PushVariable(const std::string& name)
{
	ScriptVariableData oldVarData = GetVariable(name);

	ScriptVariableData varData;
	varData.type = oldVarData.type;
	stack += varData.type.size;

	varData.target.lvalue = false;
	varData.target.offset = -stack;
	varData.target.mode = 0b01;
	if (varData.target.offset < -128)
		varData.target.mode = 0b10;
	varData.target.regm = 0b101;

	if (scope.back().vars.find(name) != scope.back().vars.end())
		throw std::runtime_error("Redefinition of variable '" + name + "'.");

	StreamAssignee<uint8_t> po(*this);
	StreamAssignee<uint8_t> o(*this);
	StreamAssignee<uint8_t> dat8(*this);
	StreamAssignee<uint32_t> dat32(*this);

	if (oldVarData.target.mode == 0b11)
	{
		sasm.Move(0x89, varData.target, oldVarData.target);
	}
	else
	{
		throw std::runtime_error("Cannot perform this move currently.");
	}

	scope.back().vars[name] = varData;
}

void ScriptCompile::PushVariable(const std::string& name, ScriptTypeData type)
{
	ScriptVariableData varData;
	varData.type = type;
	size_t var_size = type.GetSize();
	stack += var_size;

	varData.target.lvalue = false;
	varData.target.offset = -stack;
	varData.target.mode = 0b01;
	if (varData.target.offset < -128)
		varData.target.mode = 0b10;
	varData.target.regm = 0b101;

	if (scope.back().vars.find(name) != scope.back().vars.end())
		throw std::runtime_error("Redefinition of variable '" + name + "'.");

	target = varData.target;

	//StreamAssignee<uint8_t> po(*this);
	//StreamAssignee<uint8_t> o(*this);
	//StreamAssignee<uint8_t> dat8(*this);
	//StreamAssignee<uint32_t> dat32(*this);

	//if (var_size < 128)
	//{
	//	// sub esp, type.size
	//	po = 0x83;
	//	o = 0b11101100;
	//	dat8 = var_size;
	//}
	//else
	//{
	//	// sub esp, type.size
	//	po = 0x81;
	//	o = 0b11101100;
	//	dat32 = var_size;
	//}

	scope.back().vars[name] = varData;
}

void ScriptCompile::PushVariable(const std::string& name, ScriptTypeData type, ScriptCompileMemoryTarget from)
{
	ScriptVariableData varData;
	varData.type = type;
	size_t var_size = type.GetSize();
	if (var_size > 8)
		throw std::runtime_error("Can't initialize non-primitive types.");
	stack += var_size;
	
	varData.target.lvalue = false;
	varData.target.offset = -stack;
	varData.target.mode = 0b01;
	if (varData.target.offset < -128)
		varData.target.mode = 0b10;
	varData.target.regm = 0b101;

	if (scope.back().vars.find(name) != scope.back().vars.end())
		throw std::runtime_error("Redefinition of variable '" + name + "'.");

	target = varData.target;

	//StreamAssignee<uint8_t> po(*this);
	//StreamAssignee<uint8_t> o(*this);
	//StreamAssignee<uint8_t> dat8(*this);
	//StreamAssignee<uint32_t> dat32(*this);

	//if (target.mode == 0b11)
	//{
	//	// push target
	//	po = 0x50 + target.regm;
	//}
	//else
	//{
	//	// push target
	//	po = 0xff;
	//	o = target.GetModRegRM(6);
	//	if (target.mode == 0b01)
	//		dat8 = target.offset;
	//	if (target.mode == 0b10)
	//		dat32 = target.offset;
	//}

	scope.back().vars[name] = varData;
}

void ScriptCompile::PushVariable(const std::string& name, ScriptTypeData type, int32_t value)
{
	ScriptVariableData varData;
	varData.type = type;
	stack += type.size;

	varData.target.lvalue = false;
	varData.target.offset = -stack;
	varData.target.mode = 0b01;
	if (varData.target.offset < -128)
		varData.target.mode = 0b10;
	varData.target.regm = 0b101;

	if (scope.back().vars.find(name) != scope.back().vars.end())
		throw std::runtime_error("Redefinition of variable '" + name + "'.");

	StreamAssignee<uint8_t> p(*this);
	StreamAssignee<uint8_t> po(*this);
	StreamAssignee<uint8_t> o(*this);
	StreamAssignee<uint8_t> dat8(*this);
	StreamAssignee<uint32_t> dat32(*this);
	StreamAssignee<uint64_t> dat64(*this);

	target = varData.target;

	p = 0b01001000;
	po = 0xc7;
	if (target.offset >= -128)
	{
		o = 0b01000101;
		dat8 = target.offset;
	}
	else
	{
		o = 0b10000101;
		dat32 = target.offset;
	}
	dat32 = value;

	scope.back().vars[name] = varData;
}

void ScriptCompile::AddParameterStack(const std::string& name, ScriptTypeData type, int32_t offset)
{
	ScriptVariableData varData;
	varData.type = type;

	varData.target.lvalue = false;
	varData.target.offset = offset;
	varData.target.mode = 0b01;
	if (varData.target.offset < -128)
		varData.target.mode = 0b10;
	varData.target.regm = 0b101;

	if (scope.back().vars.find(name) != scope.back().vars.end())
		throw std::runtime_error("Redefinition of parameter '" + name + "'.");

	scope.back().vars[name] = varData;
}

void ScriptCompile::AddParameterRegister(const std::string& name, ScriptTypeData type, uint8_t reg)
{
	ScriptVariableData varData;
	varData.type = type;

	varData.target.lvalue = false;
	varData.target.offset = 0;
	varData.target.mode = 0b11;
	varData.target.regm = reg;

	if (scope.back().vars.find(name) != scope.back().vars.end())
		throw std::runtime_error("Redefinition of parameter '" + name + "'.");

	scope.back().vars[name] = varData;
}

ScriptVariableData ScriptCompile::GetVariable(const std::string& name)
{
	for (auto i = scope.rbegin(); i != scope.rend(); ++i)
	{
		auto var = i->vars.find(name);
		if (var != i->vars.end())
			return var->second;
	}
	if (current_class)
	{
		auto var = current_class->GetMember(name);
		if (var != ScriptVariableData())
			return var;
	}
	throw std::runtime_error("Couldn't find identifier '" + name + "'.");
}

void ScriptCompile::SetClass(const std::string& name)
{
	if (classes.find(name) != classes.end())
	{
		current_class = classes.find(name)->second;
	}
	else
	{
		current_class.reset(new ScriptClassData());
		current_class->class_name = name;
		classes.insert(std::make_pair(name, current_class));
	}
}

bool ScriptCompile::IsBusy(uint8_t reg)
{
	return busy_registers.find(reg) != busy_registers.end();
}

bool ScriptCompile::IsFree(uint8_t reg)
{
	return busy_registers.find(reg) == busy_registers.end();
}

void ScriptCompile::SetBusy(uint8_t reg)
{
	if (busy_registers.find(reg) != busy_registers.end())
		throw std::runtime_error("Register is already busy.");
	busy_registers.insert(reg);
}

void ScriptCompile::SetFree(uint8_t reg)
{
	if (busy_registers.find(reg) == busy_registers.end())
		throw std::runtime_error("Register is already free.");
	busy_registers.erase(reg);
}