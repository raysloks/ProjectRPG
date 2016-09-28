#include "Variable.h"

#ifndef STRING_VAR_H
#define STRING_VAR_H

class StringVar :
	public Variable
{
public:
	StringVar(const std::string& str);
	~StringVar(void);

	Variable * clone(void) const;

	std::shared_ptr<Variable> operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate = true);

	std::string str;
};

#endif