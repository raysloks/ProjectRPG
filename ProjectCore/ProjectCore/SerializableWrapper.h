#pragma once

#include "streams.h"

template <class T>
class SerializableWrapper
{
public:
	SerializableWrapper()
	{
		ptr = nullptr;
	}
	SerializableWrapper(const SerializableWrapper<T>& rhs)
	{
		if (rhs.ptr)
			ptr = T::_registry.getFactory(rhs.ptr->getSerial())->create(rhs.ptr);
		else
			ptr = nullptr;
	}
	SerializableWrapper(SerializableWrapper<T>&& rhs)
	{
		ptr = rhs.ptr;
		rhs.ptr = nullptr;
	}
	SerializableWrapper(T * p)
	{
		ptr = p;
	}
	~SerializableWrapper()
	{
		if (ptr)
			delete ptr;
	}

	SerializableWrapper<T>& operator=(SerializableWrapper<T>&& rhs)
	{
		ptr = rhs.ptr;
		rhs.ptr = nullptr;
		return *this;
	}

	T * ptr;
};

template <class T>
outstream& operator<<(outstream& os, const SerializableWrapper<T>& sw)
{
	T::_registry.serialize(os, sw.ptr);
	sw.ptr->write_to(os);
	return os;
}

template <class T>
instream& operator>>(instream& is, SerializableWrapper<T>& sw)
{
	auto factory = T::_registry.deserialize(is);
	if (factory)
		sw.ptr = factory->create(is, false);
	return is;
}