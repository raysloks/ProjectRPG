#include "StringVar.h"

StringVar::StringVar(const std::string& s) : str(s)
{
}

StringVar::~StringVar(void)
{
}

Variable * StringVar::clone(void) const
{
	return new StringVar(str);
}

std::shared_ptr<Variable> StringVar::operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate)
{
	if (allocate)
		return Variable::operate(op, rhs);
	else
		return 0;
}