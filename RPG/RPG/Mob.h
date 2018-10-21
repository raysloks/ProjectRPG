#pragma once

#include "Vec2.h"

class Scene;

class Mob
{
public:
	Mob();
	~Mob();

	void move(const Vec2& d, Scene& scene);
	void move(float dx, float dy, Scene& scene);
	void moveHor(float dx, Scene& scene);
	void moveVer(float dy, Scene& scene);

	union
	{
		Vec2 p;
		struct
		{
			float x, y;
		};
	};
	float r;
};