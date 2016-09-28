#include <string>
#include <memory>

#ifndef VARIABLE_H
#define VARIABLE_H

class Variable :
	public std::enable_shared_from_this<Variable>
{
public:
	Variable(void);
	virtual ~Variable(void);

	virtual Variable* clone() const;

	virtual std::shared_ptr<Variable> operate(const std::string& op, std::shared_ptr<Variable> rhs, bool allocate = true);

	std::weak_ptr<std::shared_ptr<Variable>> address;
};

#endif