#include "Variable.h"

#ifndef BOOLEAN_VAR_H
#define BOOLEAN_VAR_H

class BooleanVar :
	public Variable
{
public:
	BooleanVar(bool b = false);
	~BooleanVar(void);

	Variable * clone(void) const;

	std::shared_ptr<Variable> operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate = true);

	bool b;
};

#endif