#pragma once

#include <vector>

#include "EntityID.h"
#include "DamageData.h"

class HitData
{
public:
	HitData();
	HitData(int32_t amount);
	HitData(int32_t amount, EntityID source);
	~HitData();

	int32_t getTotal() const;

	EntityID source;
	std::vector<DamageData> damage;
};