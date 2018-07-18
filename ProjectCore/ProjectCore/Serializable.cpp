#include "Serializable.h"
#include <map>
#include <iostream>

AutoNullFactory::AutoNullFactory(void)
{
	factory = new NullFactory();
	Serializable::Register("null", 0, factory);
}

AutoNullFactory::~AutoNullFactory(void)
{
	delete factory;
}

std::map<uint32_t, StreamFactory<Serializable>*> * _reg;
std::map<uint32_t, std::string> * _names;

const AutoNullFactory Serializable::null_factory;

Serializable::Serializable(uint32_t id)
{
	_serID = id;
}

Serializable::~Serializable(void)
{
}

void Serializable::Register(const std::string& name, const size_t& id, StreamFactory<Serializable> * factory)
{
	if (_reg==0)
		_reg = new std::map<uint32_t, StreamFactory<Serializable>*>;
	if (_names==0)
		_names = new std::map<uint32_t, std::string>;
	if ((*_reg)[id]!=0)// && name.compare((*_names)[id])!=0)
		throw std::runtime_error("Hash collision!");
	(*_reg)[id] = factory;
	(*_names)[id] = name;
}

void Serializable::serialize(outstream& os, const Serializable * const instance)
{
	os << instance->_serID;
}

const StreamFactory<Serializable> * Serializable::deserialize(instream& is)
{
	uint32_t id;
	is >> id;
	if ((*_reg)[id]!=0)
		return (*_reg)[id];
	else
		return 0;
}

const StreamFactory<Serializable> * Serializable::getFactory(uint32_t id)
{
	return (*_reg)[id];
}

const std::string& Serializable::getName(uint32_t id)
{
	return _names->at(id);
}