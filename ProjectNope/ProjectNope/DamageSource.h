#ifndef DAMAGE_SOURCE_H
#define DAMAGE_SOURCE_H

#include "Vec3.h"

class Entity;

class DamageSource
{
public:
	DamageSource(Entity * source, int damage, const Vec3& knockback = Vec3());
	~DamageSource(void);

	Entity * source;
	int damage;
	Vec3 knockback;
};

#endif