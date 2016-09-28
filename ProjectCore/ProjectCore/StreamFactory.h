#include "streams.h"

#ifndef STREAM_FACTORY_H
#define STREAM_FACTORY_H

template <class T>
class StreamFactory
{
public:
	virtual T * create(instream&, bool) const
	{
		return 0;
	}

	virtual T * create(const T*) const
	{
		return 0;
	}
};

#endif