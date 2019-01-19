#pragma once

#include <Vec3.h>

class HitboxData
{
public:
	HitboxData();
	~HitboxData();

	Vec3 offset;
	float radius;
	std::string bone, activation;
};