#include "Variable.h"
#include <map>
#include <vector>

#ifndef SCRIPT_MEMORY_H
#define SCRIPT_MEMORY_H

class Script;

typedef std::map<std::string, std::shared_ptr<std::shared_ptr<Variable>>> MemoryLayer;

class ScriptMemory
{
public:
	ScriptMemory(void);
	~ScriptMemory(void);

	std::shared_ptr<Variable> getVariable(const std::string& name, bool top = false);
	std::shared_ptr<Variable> setVariable(const std::string& name, std::shared_ptr<Variable> variable, bool local = false, bool top = false);

	void pop(void);
	void push(void);
	void push(const std::shared_ptr<MemoryLayer>& layer);

	//maybe shouldn't be void? dunno
	void get(const std::string& name, float& f);
	void get(const std::string& name, std::string& str);

	std::vector<std::shared_ptr<MemoryLayer>> var;
	std::shared_ptr<Variable> return_var;
};

#endif