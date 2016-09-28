#include "FunctionVar.h"

FunctionVar::FunctionVar(void)
{
}

FunctionVar::~FunctionVar(void)
{
}

Variable * FunctionVar::clone(void) const
{
	return new FunctionVar(*this);
}

#include "VectorVar.h"

std::shared_ptr<Variable> FunctionVar::operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate)
{
	if (op.compare("()")==0)
	{
		std::vector<std::shared_ptr<Variable>> vec;
		if (rhs!=0)
		{
			std::shared_ptr<VectorVar> vv = std::dynamic_pointer_cast<VectorVar>(rhs);
			if (vv!=0)
			{
				vec.insert(vec.end(), vv->v.begin(), vv->v.end());
			}
			else
			{
				vec.insert(vec.end(), rhs);
			}
		}
		if (script_func!=0)
		{
			return script_func->run(vec);
		}
		if (func_ptr)
		{
			return func_ptr(vec);
		}
	}
	if (allocate)
		return Variable::operate(op, rhs);
	else
		return 0;
}