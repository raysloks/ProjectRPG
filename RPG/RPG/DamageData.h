#pragma once

#include "DamageTypeData.h"

class DamageData
{
public:
	DamageData();
	DamageData(int32_t amount);
	~DamageData();

	int32_t amount;
	DamageTypeData type;
};

