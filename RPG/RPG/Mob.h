#pragma once

#include "Vec3.h"

class Scene;

class Mob
{
public:
	Mob();
	~Mob();

	void move_velocity(const Vec3& d, Scene& scene);
	void move(const Vec3& d, Scene& scene);
	void move(float dx, float dy, float dz, Scene& scene);

	union
	{
		Vec3 p;
		struct
		{
			float x, y, z;
		};
	};
	Vec3 v;
	float r;
	uint32_t land;
};