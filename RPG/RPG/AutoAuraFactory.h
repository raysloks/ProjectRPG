#pragma once

#include "AuraFactory.h"

#include <tuple>

template <class T, auto... Args>
class AutoAuraFactory :
	public AuraFactory
{
public:
	Aura * create() const
	{
		return new T(Args...);
	}

	Aura * create(const Aura * original) const
	{
		return new T(*(const T*)original);
	}
};