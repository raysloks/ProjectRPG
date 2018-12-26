#ifndef AUTO_SERIAL_FACTORY_H
#define AUTO_SERIAL_FACTORY_H

#include "SerialFactory.h"
#include "SerialHash.h"
#include "TypeIterator.h"

#include <string>

#define ASF_H(x, y) static AutoSerialFactory<x, y> _factory;
#define ASF_C(x, y) AutoSerialFactory<x, y> x::_factory(#x);

template <class T, class Base>
class AutoSerialFactory
{
public:
	AutoSerialFactory(const std::string& name)
	{
		id = sdbm(name);
		factory = new SerialFactory<T, Base>();
		auto queue = Base::_getRegistrationQueue();
		if (queue->registry)
			Base::_registry.registerFactory(id, factory);
		else
			queue->queue.push_back(std::make_pair(id, factory));
	}
	~AutoSerialFactory(void)
	{
		delete factory;
	}

	StreamFactory<Base> * factory;
	uint32_t id;
};

#endif