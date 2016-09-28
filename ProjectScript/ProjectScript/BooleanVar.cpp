#include "BooleanVar.h"

BooleanVar::BooleanVar(bool value) : b(value)
{
}

BooleanVar::~BooleanVar(void)
{
}

Variable * BooleanVar::clone(void) const
{
	return new BooleanVar(b);
}

std::shared_ptr<Variable> BooleanVar::operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate)
{
	std::shared_ptr<BooleanVar> vb = std::dynamic_pointer_cast<BooleanVar>(rhs);
	if (vb!=0)
	{
		switch(op.front())
		{
		case'=':
			if (op.back()=='=' && op.size()==2) {
				return std::shared_ptr<Variable>(new BooleanVar(b==vb->b));
			} else {
				b=vb->b;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			}
		case'!':
			if (op.back()=='=') {
				return std::shared_ptr<Variable>(new BooleanVar(b!=vb->b));
			}
		case'&':
			return std::shared_ptr<Variable>(new BooleanVar(b&&vb->b));
		case'|':
			return std::shared_ptr<Variable>(new BooleanVar(b||vb->b));
		}
	}
	else
	{
		if (op.compare("!")==0)
			return std::shared_ptr<Variable>(new BooleanVar(!b));
	}
	if (allocate)
		return Variable::operate(op, rhs);
	else
		return 0;
}