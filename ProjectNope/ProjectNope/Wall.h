#include "Vec3.h"
#include "Collision.h"
#include <memory>

#ifndef WALL_H
#define WALL_H

class Entity;

class Wall
{
public:
	Wall(const Vec3& vP1, const Vec3& vP2, const Vec3& vP3);
	~Wall(void);

	void Recalculate(void);

	std::pair<Vec3, Vec3> GetAABB(void);

	std::shared_ptr<Collision> SphereCheck(const Vec3& vP, float r);
	std::shared_ptr<Collision> LineCheck(const Vec3& sP, const Vec3& eP);
	std::shared_ptr<Collision> LineCheck(const Vec3& sP, const Vec3& eP, float r);

	static std::shared_ptr<Collision> TubeLine(const Vec3& sP, const Vec3& eP, const Vec3& vP, const Vec3& dir, float r);
	static std::shared_ptr<Collision> DiskCastLine(const Vec3& sP, const Vec3& eP, const Vec3& vP, const Vec3& dir, float cl, float r);
	static std::shared_ptr<Collision> SphereLine(const Vec3& sP, const Vec3& eP, const Vec3& vP, float r);

	std::shared_ptr<Collision> SphereCast(const Vec3& sP, const Vec3& eP, float r);

	std::shared_ptr<Collision> DiskCast(const Vec3& sP, const Vec3& eP, float r);
	std::shared_ptr<Collision> LowerDisk(const Vec3& lock, const Vec3& center, const Vec3& axis, const Vec3& dir, float r);

	Vec3 p1,p2,p3,n,n12,n23,n31;
	float l12,l23,l31;
};

#endif