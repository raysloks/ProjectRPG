#include "FloatVar.h"
#include "BooleanVar.h"

FloatVar::FloatVar(float value)
{
	f = value;
}

FloatVar::FloatVar(const FloatVar& var)
{
	f = var.f;
}

FloatVar::~FloatVar(void)
{
}

Variable * FloatVar::clone(void) const
{
	return new FloatVar(f);
}

std::shared_ptr<Variable> FloatVar::operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate)
{
	std::shared_ptr<FloatVar> vf = std::dynamic_pointer_cast<FloatVar>(rhs);
	if (vf!=0)
	{
		switch(op.front())
		{
		case'+':
			if (op.back()=='=') {
				f += vf->f;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new FloatVar(f+vf->f));
			}
		case'-':
			if (op.back()=='=') {
				f -= vf->f;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new FloatVar(f-vf->f));
			}
		case'*':
			if (op.back()=='=') {
				f *= vf->f;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new FloatVar(f*(vf->f)));
			}
		case'/':
			if (op.back()=='=') {
				f /= vf->f;
				return *std::shared_ptr<std::shared_ptr<Variable>>(address);
			} else {
				return std::shared_ptr<Variable>(new FloatVar(f/vf->f));
			}
		case'<':
			if (op.back()=='=')
				return std::shared_ptr<Variable>(new BooleanVar(f<=vf->f));
			else
				return std::shared_ptr<Variable>(new BooleanVar(f<vf->f));
		case'>':
			if (op.back()=='=')
				return std::shared_ptr<Variable>(new BooleanVar(f>=vf->f));
			else
				return std::shared_ptr<Variable>(new BooleanVar(f>vf->f));
		case'=':
			if (op.back()=='=' && op.size()==2) {
				return std::shared_ptr<Variable>(new BooleanVar(f==vf->f));
			} else {
				f=vf->f;
				return shared_from_this();
			}
		case'!':
			if (op.back()=='=') {
				return std::shared_ptr<Variable>(new BooleanVar(f!=vf->f));
			}
		}
	}
	else
	{
		if (op.compare("-")==0)
			return std::shared_ptr<Variable>(new FloatVar(-f));
	}
	if (allocate)
		return Variable::operate(op, rhs);
	else
		return 0;
}