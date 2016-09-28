#ifndef GRUNT_AI_H
#define GRUNT_AI_H

#include "UnitCapability.h"
#include "Vec2.h"
#include "Entity.h"

class GruntAI
	: public UnitCapability
{
public:
	GruntAI(Unit * unit, bool full);
	~GruntAI(void);

	void tick(float dTime);

	Vec2 move, fm;
	EntityID target_id;

	bool move_it;
};

#endif