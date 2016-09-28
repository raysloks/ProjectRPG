#include "Variable.h"
#include "GlobalPosition.h"

#ifndef POSITION_VAR_H
#define POSITION_VAR_H

class PositionVar :
	public Variable
{
public:
	PositionVar(void);
	PositionVar(const GlobalPosition& pos);
	~PositionVar(void);

	Variable * clone(void) const;

	std::shared_ptr<Variable> operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate = true);

	GlobalPosition p;
};

#endif