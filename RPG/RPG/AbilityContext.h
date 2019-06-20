#pragma once

#include "GlobalPosition.h"

class MobComponent;

class AbilityContext
{
public:
	AbilityContext();
	~AbilityContext();

	MobComponent * source;
	MobComponent * target;
	GlobalPosition position;
};