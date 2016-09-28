#include "ScriptMemory.h"
#include "Script.h"

ScriptMemory::ScriptMemory(void)
{
	push(std::shared_ptr<MemoryLayer>(new MemoryLayer()));
}

ScriptMemory::~ScriptMemory(void)
{
}

#include <iostream>

std::shared_ptr<Variable> ScriptMemory::getVariable(const std::string& name, bool top)
{
	//std::cout << "getting variable " << name << std::endl;
	if (top)
	{
		return *(*var.front())[name];
	}
	for (auto i=var.rbegin();i!=var.rend();++i) {
		if ((**i)[name]!=0) {
			return *(**i)[name];
		}
	}
	return 0;
}

std::shared_ptr<Variable> ScriptMemory::setVariable(const std::string& name, std::shared_ptr<Variable> variable, bool local, bool top)
{
	//std::cout << "setting variable " << name << std::endl;
	if (top)
	{
		(*var.front())[name] = std::shared_ptr<std::shared_ptr<Variable>>(new std::shared_ptr<Variable>(variable));
		variable->address = (*var.front())[name];
		return variable;
	}
	if (!local)
	{
		for (auto i=var.rbegin();i!=var.rend();++i)
			if ((**i).find(name)!=(**i).end())
			{
				(**i)[name] = std::shared_ptr<std::shared_ptr<Variable>>(new std::shared_ptr<Variable>(variable));
				variable->address = (**i)[name];
				return variable;
			}
	}
	(*var.back())[name] = std::shared_ptr<std::shared_ptr<Variable>>(new std::shared_ptr<Variable>(variable));
	variable->address = (*var.back())[name];
	return variable;
}

void ScriptMemory::pop(void)
{
	var.pop_back();
}

void ScriptMemory::push(void)
{
	var.push_back(std::shared_ptr<MemoryLayer>(new MemoryLayer()));
}

void ScriptMemory::push(const std::shared_ptr<MemoryLayer>& layer)
{
	var.push_back(layer);
}

#include "FloatVar.h"

void ScriptMemory::get(const std::string& name, float& f)
{
	std::shared_ptr<FloatVar> var = std::dynamic_pointer_cast<FloatVar>(getVariable(name));
	if (var!=0)
		f = var->f;
}

#include "StringVar.h"

void ScriptMemory::get(const std::string& name, std::string& str)
{
	std::shared_ptr<StringVar> var = std::dynamic_pointer_cast<StringVar>(getVariable(name));
	if (var!=0)
		str = var->str;
}