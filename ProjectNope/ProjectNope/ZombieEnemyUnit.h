#ifndef ZOMBIE_ENEMY_UNIT_H
#define ZOMBIE_ENEMY_UNIT_H

#include "Unit.h"

class AnimatedCollisionMesh;

class ZombieEnemyUnit :
	public Unit
{
public:
	ZombieEnemyUnit(const GlobalPosition& pos, const Vec3& vel, const std::string& mesh, const std::string& animation, const std::string& texture);
	ZombieEnemyUnit(instream& is, bool full);
	~ZombieEnemyUnit(void);

	static const AutoSerialFactory<ZombieEnemyUnit> _factory_ZombieEnemyUnit;

	virtual void tick(float dTime);
protected:

	AnimatedCollisionMesh * acm;

	float dir;
};

#endif