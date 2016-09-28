#include "Variable.h"
#include <vector>

#ifndef VECTOR_VAR_H
#define VECTOR_VAR_H

class VectorVar :
	public Variable
{
public:
	VectorVar(void);
	~VectorVar(void);

	Variable * clone(void) const;

	std::shared_ptr<Variable> operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate = true);

	std::vector<std::shared_ptr<Variable>> v;
};

#endif