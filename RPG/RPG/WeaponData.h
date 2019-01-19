#pragma once

#include <vector>

#include "HitboxData.h"
#include "Decorator.h"

class WeaponData
{
public:
	WeaponData();
	~WeaponData();

	std::vector<HitboxData> hitboxes;
	Decorator dec;
};