#pragma once

#include <vector>
#include "StreamFactory.h"

template <class Base>
class SerializableRegistry;

template <class Base>
class SerializableRegistrationQueue
{
public:
	SerializableRegistry<Base> * registry = nullptr;
	std::vector<std::pair<uint32_t, StreamFactory<Base>*>> queue;
};