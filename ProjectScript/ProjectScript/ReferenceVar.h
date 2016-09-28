#include "Variable.h"

#ifndef REFERENCE_VAR_H
#define REFERENCE_VAR_H

class ReferenceVar :
	public Variable
{
public:
	ReferenceVar(void);
	ReferenceVar(const std::shared_ptr<Variable>& ptr);
	~ReferenceVar(void);

	Variable * clone(void) const;

	std::shared_ptr<Variable> operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate = true);

	std::shared_ptr<Variable> ref;
};

#endif