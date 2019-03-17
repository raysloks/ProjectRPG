#pragma once

#include <vector>

#include "HitboxData.h"

class WeaponData
{
public:
	WeaponData();
	~WeaponData();

	std::vector<HitboxData> hitboxes;
};