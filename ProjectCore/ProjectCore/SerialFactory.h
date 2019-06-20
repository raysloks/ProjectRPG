#ifndef SERIAL_FACTORY_H
#define SERIAL_FACTORY_H

#include "StreamFactory.h"

template <class T, class Base>
class SerialFactory :
	public StreamFactory<Base>
{
public:
	Base * create(instream& is) const
	{
		return new T(is);
	}

	Base * create(instream& is, ClientData& client) const
	{
		if constexpr (std::is_constructible<T, instream&, ClientData&>::value)
			return new T(is, client);
		else
			return new T(is);
	}

	Base * create(instream& is, uint32_t version) const
	{
		if constexpr (std::is_constructible<T, instream&, uint32_t>::value)
			return new T(is, version);
		else
			return new T(is);
	}

	Base * create(const Base * instance) const
	{
		return new T(*reinterpret_cast<const T*>(instance));
	}
};

#endif