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
	
	std::shared_ptr<Factory<Aura>> aura;
};