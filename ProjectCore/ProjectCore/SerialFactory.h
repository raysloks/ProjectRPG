#ifndef SERIAL_FACTORY_H
#define SERIAL_FACTORY_H

#include "StreamFactory.h"

template <class T, class Base>
class SerialFactory : public StreamFactory<Base>
{
public:
	Base * create(instream& is, bool full) const
	{
		return new T(is, full);
	}

	Base * create(const Base * instance) const
	{
		return new T(*reinterpret_cast<const T*>(instance));
	}
};

#endif