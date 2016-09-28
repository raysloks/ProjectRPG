#include "VectorVar.h"

VectorVar::VectorVar(void)
{
}

VectorVar::~VectorVar(void)
{
}

Variable * VectorVar::clone(void) const
{
	return new VectorVar(*this);
}

#include "StringVar.h"
#include "FloatVar.h"

std::shared_ptr<Variable> VectorVar::operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate)
{
	if (op.compare(",")==0)
	{
		if (rhs!=0)
		{
			v.push_back(std::shared_ptr<Variable>(rhs->clone()));
			return shared_from_this();
		}
	}
	std::shared_ptr<StringVar> vstr = std::dynamic_pointer_cast<StringVar>(rhs);
	std::shared_ptr<FloatVar> vf = std::dynamic_pointer_cast<FloatVar>(rhs);
	switch(op.front())
	{
	case'.':
		if (vstr!=0)
		{
			if (vstr->str.compare("size")==0)
				return std::shared_ptr<FloatVar>(new FloatVar(v.size()));
			if (vstr->str.compare("front")==0)
				return v.front();
		}
		break;
	case'(':
		if (vf!=0)
		{
			if ((int)vf->f>=v.size())
				return 0;
			if ((int)vf->f<0)
				return 0;
			return v[vf->f];
		}
		break;
	default:
		break;
	}
	if (allocate)
		return Variable::operate(op, rhs);
	else
		return 0;
}