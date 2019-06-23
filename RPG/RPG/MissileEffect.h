#pragma once

#include "Effect.h"

class MissileEffect :
	public Effect
{
public:
	MissileEffect(uint32_t missile_index);
	~MissileEffect();
	
	void apply(const AbilityContext& ac) const;
	 
	uint32_t missile_index;
};