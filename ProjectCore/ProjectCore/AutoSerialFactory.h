#ifndef AUTO_SERIAL_FACTORY_H
#define AUTO_SERIAL_FACTORY_H

#include "SerialFactory.h"
#include "SerialHash.h"
#include "TypeIterator.h"

#include <string>

template <class T, class Base>
class AutoSerialFactory
{
public:
	AutoSerialFactory(const std::string& name)
	{
		id = sdbm(name);
		factory = new SerialFactory<T, Base>();
		auto queue = Base::_registry._getRegistrationQueue();
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