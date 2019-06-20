#pragma once

template <class Base>
class Factory
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
};