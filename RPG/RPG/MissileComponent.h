#pragma once

#include "Component.h"

#include "EntityID.h"
#include "GlobalPosition.h"

class AbilityContext;

class MissileComponent :
	public Component
{
public:
	static void init();
	static MissileComponent * create(uint32_t index, const AbilityContext& ac);
	static void release();
	
	void tick(float dTime);
	
	EntityID target_mob_id;
	GlobalPosition origin, target;
};