#ifndef JUMP_CAP_H
#define JUMP_CAP_H

#include "UnitCapability.h"

class JumpCap
	: public UnitCapability
{
public:
	JumpCap(Unit * unit, float power);
	~JumpCap(void);

	void tick(float dTime);

	float power, peak;
};

#endif