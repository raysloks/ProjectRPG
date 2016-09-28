#include "Variable.h"

#ifndef FLOAT_VAR_H
#define FLOAT_VAR_H

class FloatVar :
	public Variable
{
public:
	FloatVar(float f = 0.0f);
	FloatVar(const FloatVar& f);
	~FloatVar(void);

	Variable * clone(void) const;

	std::shared_ptr<Variable> operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate = true);

	float f;
};

#endif