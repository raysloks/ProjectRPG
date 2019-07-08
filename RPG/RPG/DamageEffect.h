#pragma once

#include "Effect.h"

class DamageEffect :
	public Effect
{
public:
	DamageEffect(int32_t damage);
	~DamageEffect();

	void apply(const AbilityContext& ac) const;

	int32_t damage;
};

