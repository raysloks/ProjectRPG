#include "MemoryVar.h"

MemoryVar::MemoryVar(ScriptMemory * memory) : mem(memory)
{
}

MemoryVar::~MemoryVar(void)
{
}

Variable * MemoryVar::clone(void) const
{
	return new MemoryVar(*this);
}

#include "StringVar.h"
#include "BooleanVar.h"

std::shared_ptr<Variable> MemoryVar::operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate)
{
	std::shared_ptr<MemoryVar> var = std::dynamic_pointer_cast<MemoryVar>(rhs);
	if (var!=0)
	{
		switch(op.front())
		{
		case'!':
			{
				if (op.size()==2 && op.back()=='=')
					return std::shared_ptr<BooleanVar>(new BooleanVar(mem!=var->mem));
			}
			break;
		case'=':
			{
				if (op.size()==2 && op.back()=='=')
					return std::shared_ptr<BooleanVar>(new BooleanVar(mem==var->mem));
			}
			break;
		default:
			break;
		}
	}
	if (op.compare(".")==0)
	{
		std::shared_ptr<StringVar> str = std::dynamic_pointer_cast<StringVar>(rhs);
		if (str!=0)
		{
			if (mem!=0)
			{
				std::shared_ptr<Variable> var = mem->getVariable(str->str);
				if (var!=0)
					return var;
				else
					return mem->setVariable(str->str, std::shared_ptr<Variable>(new Variable()));
			}
		}
	}
	if (allocate)
		return Variable::operate(op, rhs);
	else
		return 0;
}