#ifndef COLLISION_H
#define COLLISION_H

#include "GlobalPosition.h"

class Component;

class Collision
{
public:
	Collision(void);

	GlobalPosition poo, poc;
	Vec3 n, disp, v;
	float dist, t;
	Component * comp;
	void * ce;
};

#endif