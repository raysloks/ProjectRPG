#ifndef TARGET_LOCK_CAP_H
#define TARGET_LOCK_CAP_H

#include "UnitCapability.h"
#include "Entity.h"

class TargetLockCap
	: public UnitCapability
{
public:
	TargetLockCap(Unit * unit);
	~TargetLockCap(void);

	void tick(float dTime);

	EntityID target_id;
};

#endif