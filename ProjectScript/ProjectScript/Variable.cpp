#include "Variable.h"
#include "ReferenceVar.h"
#include "VectorVar.h"

Variable::Variable(void)
{
}

Variable::~Variable(void)
{
}

Variable* Variable::clone() const
{
	return new Variable();
}

std::shared_ptr<Variable> Variable::operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate)
{
	if (op.compare("=")==0)
	{
		if (rhs!=0)
		{
			std::shared_ptr<Variable> var(rhs->clone());
			var->address = address;
			(*std::shared_ptr<std::shared_ptr<Variable>>(address))=var;
			return var;
		}
	}
	if (op.compare("&")==0)
	{
		if (rhs!=0)
		{
			return std::shared_ptr<Variable>(new ReferenceVar(shared_from_this()));
		}
	}
	if (op.compare(",")==0)
	{
		if (rhs!=0)
		{
			std::shared_ptr<VectorVar> vec(new VectorVar());
			vec->v.push_back(std::shared_ptr<Variable>(this->clone()));
			vec->v.push_back(std::shared_ptr<Variable>(rhs->clone()));
			return vec;
		}
	}
	return 0;
}