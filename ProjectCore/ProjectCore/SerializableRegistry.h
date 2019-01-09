#pragma once

#include <unordered_map>

#include "StreamFactory.h"
#include "SerializableRegistrationQueue.h"

template <class Base>
class SerializableRegistry
{
public:
	SerializableRegistry()
	{
		registerFactory(0, new StreamFactory<Base>());
		auto queue = _getRegistrationQueue();
		queue->registry = this;
		for (auto factory : queue->queue)
		{
			registerFactory(factory.first, factory.second);
		}
	}

	void registerFactory(uint32_t id, StreamFactory<Base> * factory)
	{
		if (reg[id])
			throw std::runtime_error(std::string("SerializableRegistry<") + typeid(Base).name() + "> hash collision.");
		reg[id] = factory;
	}

	void serialize(outstream& os, Base * instance)
	{
		os << instance->getSerial();
	}

	StreamFactory<Base> * deserialize(instream& is)
	{
		uint32_t id;
		is >> id;
		return getFactory(id);
	}

	StreamFactory<Base> * getFactory(uint32_t id)
	{
		return reg[id];
	}

	static SerializableRegistrationQueue<Base> * _getRegistrationQueue()
	{
		static SerializableRegistrationQueue<Base> * rq = new SerializableRegistrationQueue<Base>();
		return rq;
	}

	std::unordered_map<uint32_t, StreamFactory<Base>*> reg;
};