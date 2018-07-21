#pragma once

#include "GlobalPosition.h"

class MobComponent;
class World;

class GolemUnit
{
public:
	GolemUnit() = delete;

	static MobComponent * spawn(const GlobalPosition& pos, World * world);
};