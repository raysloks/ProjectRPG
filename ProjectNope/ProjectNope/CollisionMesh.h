#include <vector>
#include "DynamicWall.h"

#ifndef COLLISION_MESH_H
#define COLLISION_MESH_H

class Mesh;
class Matrix4;

class CollisionMesh
{
public:
	CollisionMesh(void);
	CollisionMesh(const std::string& mesh);
	virtual ~CollisionMesh(void);

	bool LoadFromResource(const std::string& mesh);

	void Recalculate(const Mesh& mesh, const Mesh& next_mesh, float dTime);
	void Recalculate(const Mesh& mesh, const Mesh& next_mesh, const Matrix4& mtrx, const Matrix4& next_mtrx, float dTime);
	void Recalculate(void);
	void CalcBounds(void);
	void CalcGrid(void);

	size_t GetCell(Vec3 vP) const;
	std::vector<size_t> GetCellsInAABB(Vec3 box_min, Vec3 box_max) const;
	std::vector<size_t> GetWallsInAABB(Vec3 box_min, Vec3 box_max) const;

	void SphereCheck(const Vec3& vP, float r, std::vector<std::shared_ptr<Collision>>& list);
	void LineCheck(const Vec3& sP, const Vec3& eP, std::vector<std::shared_ptr<Collision>>& list);

	void SphereCast(const Vec3& sP, const Vec3& eP, float r, std::vector<std::shared_ptr<Collision>>& list);
	void DiskCast(const Vec3& sP, const Vec3& eP, float r, std::vector<std::shared_ptr<Collision>>& list);
	void LowerDisk(const Vec3& lock, const Vec3& center, const Vec3& axis, const Vec3& dir, float r, std::vector<std::shared_ptr<Collision>>& list);

	void debug_render(void);

	std::vector<DynamicWall> walls;
	std::vector<std::vector<DynamicWall>> grid;
	size_t grid_w, grid_h, grid_d;
	size_t grid_wh;
	Vec3 aabb_min, aabb_max, dif;
};

#endif