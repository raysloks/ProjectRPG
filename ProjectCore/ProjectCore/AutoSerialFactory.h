#ifndef AUTO_SERIAL_FACTORY_H
#define AUTO_SERIAL_FACTORY_H

#include "Serializable.h"
#include "TypeIterator.h"

#include <string>

template <class T>
class AutoSerialFactory
{
public:
	AutoSerialFactory(const std::string& class_name) : name(class_name)
	{
		std::hash<std::string> hash;
		id = hash(name);
		factory = new SerialFactory<T>();
		Serializable::Register(name, id, factory);
	}
	~AutoSerialFactory(void)
	{
		delete factory;
	}

	StreamFactory<Serializable> * factory;
	std::string name;
	size_t id;
};

#endif