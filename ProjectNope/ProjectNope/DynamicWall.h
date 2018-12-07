#include "Wall.h"

#ifndef DYNAMIC_WALL_H
#define DYNAMIC_WALL_H

Vec3 calcWeight(const Vec3& p1, const Vec3& p2, const Vec3& p3, const Vec3& p);

class DynamicWall :
	public Wall
{
public:
	DynamicWall(const Vec3& vP1, const Vec3& vP2, const Vec3& vP3);
	~DynamicWall(void);

	std::shared_ptr<Collision> SphereCheck(const Vec3& vP, float r);
	std::shared_ptr<Collision> LineCheck(const Vec3& sP, const Vec3& eP);

	std::shared_ptr<Collision> SphereCast(const Vec3& sP, const Vec3& eP, float r);

	Vec3 getWeight(const Vec3& p)const;
	void applyVelocity(const std::shared_ptr<Collision>& col)const;
	bool validate(const Vec3& sP, const Vec3& dif, float t)const;

	void * id;
	Vec3 vV1, vV2, vV3;
};

#endif