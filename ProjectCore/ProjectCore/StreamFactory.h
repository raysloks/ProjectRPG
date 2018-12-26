#include "streams.h"

#ifndef STREAM_FACTORY_H
#define STREAM_FACTORY_H

template <class Base>
class StreamFactory
{
public:
	virtual Base * create(instream&, bool) const
	{
		return nullptr;
	}

	virtual Base * create(const Base *) const
	{
		return nullptr;
	}
};

#endif