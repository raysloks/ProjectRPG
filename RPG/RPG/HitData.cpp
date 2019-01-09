#include "HitData.h"

HitData::HitData()
{
}

HitData::HitData(int32_t amount)
{
	damage.push_back(DamageData(amount));
}

HitData::HitData(int32_t amount, EntityID src) : HitData(amount)
{
	source = src;
}

HitData::~HitData()
{
}

int32_t HitData::getTotal() const
{
	int32_t sum = 0;
	for (auto dmg : damage)
	{
		sum += dmg.amount;
	}
	return sum;
}
