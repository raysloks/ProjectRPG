#pragma once

#include "AuraFactory.h"

#include <tuple>

template <class T, auto... Args>
class AutoAuraFactory :
	public AuraFactory
{
public:
	AutoAuraFactory(const std::string& icon) : icon(icon) {}

	Aura * create() const
	{
		Aura * aura = new T(Args...);
		aura->icon = icon;
		return aura;
	}

	Aura * create(const Aura * original) const
	{
		return new T(*(const T*)original);
	}

	std::string icon;
};