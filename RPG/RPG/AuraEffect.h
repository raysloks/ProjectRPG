#pragma once

#include "Effect.h"

#include "Factory.h"
#include "Aura.h"

class AuraEffect :
	public Effect
{
public:
	AuraEffect(uint32_t aura_index);
	~AuraEffect();
	
	void apply(const AbilityContext& ac) const;
	
	uint32_t aura_index;
};