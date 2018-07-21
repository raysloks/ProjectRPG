#pragma once

#include "GlobalPosition.h"

class MobComponent;
class World;

class ShadowSpawnUnit
{
public:
	ShadowSpawnUnit() = delete;

	static MobComponent * spawn(const GlobalPosition& pos, World * world);
};