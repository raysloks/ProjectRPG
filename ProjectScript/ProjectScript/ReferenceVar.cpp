#include "ReferenceVar.h"

ReferenceVar::ReferenceVar(void)
{
}

ReferenceVar::ReferenceVar(const std::shared_ptr<Variable>& ptr) : ref(ptr)
{
}

ReferenceVar::~ReferenceVar(void)
{
}

Variable * ReferenceVar::clone(void) const
{
	return new ReferenceVar(*this);
}

std::shared_ptr<Variable> ReferenceVar::operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate)
{
	if (op.compare("*")==0 && rhs==0)
		return ref;
	std::shared_ptr<Variable> var = ref->operate(op, rhs, false);
	if (var==0)
		return Variable::operate(op, rhs);
	return var;
}