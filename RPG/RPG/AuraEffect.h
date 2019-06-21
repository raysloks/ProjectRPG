#pragma once

#include "Effect.h"

#include "Factory.h"
#include "Aura.h"

class AuraEffect :
	public Effect
{
public:
	~AuraEffect();
	
	void apply(const AbilityContext& ac);
	
	uint32_t aura_index;
};