#pragma once

#include "Component.h"

class MissileComponent :
	public Component
{
public:
	static void init();
	static Missile * create(uint32_t index, const AbilityContext& ac);
	static void release();
	
	void tick(float dTime);
	
	EntityID target_mob_id;
	GlobalPosition origin, target;
};