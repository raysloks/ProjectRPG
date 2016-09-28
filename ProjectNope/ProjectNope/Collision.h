#ifndef COLLISION_H
#define COLLISION_H

#include "GlobalPosition.h"

class ColliderComponent;

class Collision
{
public:
	Collision(void);

	GlobalPosition poo, poc;
	Vec3 n, disp, v;
	float dist, t;
	ColliderComponent * comp;
	void * ce;
};

#endif