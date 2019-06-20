#pragma once

#include "Factory.h"

#include "streams.h"

class ClientData;

template <class Base>
class StreamFactory :
	public Factory<Base>
{
public:
	virtual Base * create() const
	{
		return nullptr;
	}

	virtual Base * create(const Base * original) const
	{
		return nullptr;
	}

	virtual Base * create(instream& is) const
	{
		return nullptr;
	}

	virtual Base * create(instream& is, ClientData& client) const
	{
		return nullptr;
	}

	virtual Base * create(instream& is, uint32_t version) const
	{
		return nullptr;
	}
};