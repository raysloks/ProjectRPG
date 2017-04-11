#include "CollisionMesh.h"
#include "GUIObject.h"
#include "Resource.h"
#include "Mesh.h"

CollisionMesh::CollisionMesh(void)
{
}

CollisionMesh::CollisionMesh(const std::string& mesh)
{
	LoadFromResource(mesh);
}

CollisionMesh::~CollisionMesh(void)
{
}

bool CollisionMesh::LoadFromResource(const std::string& mesh)
{
	std::shared_ptr<Mesh> pMesh = Resource::get<Mesh>(mesh);
	if (pMesh!=0)
	{
		walls.clear();
		for (auto set = pMesh->sets.begin();set!=pMesh->sets.end();++set)
		{
			for (auto it = set->vertices.begin();it!=set->vertices.end();++it)
			{
				walls.push_back(DynamicWall(pMesh->vert[it->a].p,pMesh->vert[it->b].p,pMesh->vert[it->c].p));
			}
		}
		CalcBounds();
		CalcGrid();
		return true;
	}
	return false;
}

void CollisionMesh::Recalculate(const Mesh& mesh, const Mesh& next_mesh, float dTime)
{
	if (walls.size()==0)
	{
		for (auto set = mesh.sets.begin();set!=mesh.sets.end();++set)
		{
			for (auto it = set->vertices.begin();it!=set->vertices.end();++it)
			{
				walls.push_back(DynamicWall(mesh.vert[it->a].p, mesh.vert[it->b].p, mesh.vert[it->c].p));
				walls.back().vV1 = (next_mesh.vert[it->a].p-mesh.vert[it->a].p)/dTime;
				walls.back().vV2 = (next_mesh.vert[it->b].p-mesh.vert[it->b].p)/dTime;
				walls.back().vV3 = (next_mesh.vert[it->c].p-mesh.vert[it->c].p)/dTime;
			}
		}
	}
	else
	{
		auto wall = walls.begin();
		for (auto set = mesh.sets.begin();set!=mesh.sets.end();++set)
		{
			for (auto it = set->vertices.begin();it!=set->vertices.end();++it)
			{
				wall->p1 = mesh.vert[it->a].p;
				wall->p2 = mesh.vert[it->b].p;
				wall->p3 = mesh.vert[it->c].p;
				wall->Recalculate();
				wall->vV1 = (next_mesh.vert[it->a].p-mesh.vert[it->a].p)/dTime;
				wall->vV2 = (next_mesh.vert[it->b].p-mesh.vert[it->b].p)/dTime;
				wall->vV3 = (next_mesh.vert[it->c].p-mesh.vert[it->c].p)/dTime;
				++wall;
			}
		}
	}
	CalcBounds();
	CalcGrid();
}

void CollisionMesh::Recalculate(const Mesh& mesh, const Mesh& next_mesh, const Matrix4& mtrx, const Matrix4& next_mtrx, float dTime)
{
	if (walls.size()==0)
	{
		for (auto set = mesh.sets.begin();set!=mesh.sets.end();++set)
		{
			for (auto it = set->vertices.begin();it!=set->vertices.end();++it)
			{
				walls.push_back(DynamicWall(mesh.vert[it->a].p*mtrx, mesh.vert[it->b].p*mtrx, mesh.vert[it->c].p*mtrx));
				walls.back().vV1 = (next_mesh.vert[it->a].p*next_mtrx-mesh.vert[it->a].p*mtrx)/dTime;
				walls.back().vV2 = (next_mesh.vert[it->b].p*next_mtrx-mesh.vert[it->b].p*mtrx)/dTime;
				walls.back().vV3 = (next_mesh.vert[it->c].p*next_mtrx-mesh.vert[it->c].p*mtrx)/dTime;
			}
		}
	}
	else
	{
		auto wall = walls.begin();
		for (auto set = mesh.sets.begin();set!=mesh.sets.end();++set)
		{
			for (auto it = set->vertices.begin();it!=set->vertices.end();++it)
			{
				wall->p1 = mesh.vert[it->a].p*mtrx;
				wall->p2 = mesh.vert[it->b].p*mtrx;
				wall->p3 = mesh.vert[it->c].p*mtrx;
				wall->Recalculate();
				wall->vV1 = (next_mesh.vert[it->a].p*next_mtrx-mesh.vert[it->a].p*mtrx)/dTime;
				wall->vV2 = (next_mesh.vert[it->b].p*next_mtrx-mesh.vert[it->b].p*mtrx)/dTime;
				wall->vV3 = (next_mesh.vert[it->c].p*next_mtrx-mesh.vert[it->c].p*mtrx)/dTime;
				++wall;
			}
		}
	}
	CalcBounds();
	CalcGrid();
}

void CollisionMesh::Recalculate(void)
{
	for (auto i = walls.begin(); i != walls.end(); ++i)
	{
		i->Recalculate();
	}
	CalcBounds();
	CalcGrid();

	/*float f;
	bounds = 0.0f;
	for (auto i=walls.begin();i!=walls.end();++i)
	{
		i->Recalculate();
		f = i->p1.LenPwr();
		if (f>bounds)
			bounds = f;
		f = i->p2.LenPwr();
		if (f>bounds)
			bounds = f;
		f = i->p3.LenPwr();
		if (f>bounds)
			bounds = f;
	}
	bounds = sqrt(bounds);*/
}

void CollisionMesh::CalcBounds(void)
{
	aabb_min = Vec3(INFINITE, INFINITE, INFINITE);
	aabb_max = -aabb_min;

	for each (auto wall in walls)
	{
		aabb_max.x = std::fmaxf(aabb_max.x, wall.p1.x);
		aabb_max.y = std::fmaxf(aabb_max.y, wall.p1.y);
		aabb_max.z = std::fmaxf(aabb_max.z, wall.p1.z);

		aabb_max.x = std::fmaxf(aabb_max.x, wall.p2.x);
		aabb_max.y = std::fmaxf(aabb_max.y, wall.p2.y);
		aabb_max.z = std::fmaxf(aabb_max.z, wall.p2.z);

		aabb_max.x = std::fmaxf(aabb_max.x, wall.p3.x);
		aabb_max.y = std::fmaxf(aabb_max.y, wall.p3.y);
		aabb_max.z = std::fmaxf(aabb_max.z, wall.p3.z);

		aabb_min.x = std::fminf(aabb_min.x, wall.p1.x);
		aabb_min.y = std::fminf(aabb_min.y, wall.p1.y);
		aabb_min.z = std::fminf(aabb_min.z, wall.p1.z);

		aabb_min.x = std::fminf(aabb_min.x, wall.p2.x);
		aabb_min.y = std::fminf(aabb_min.y, wall.p2.y);
		aabb_min.z = std::fminf(aabb_min.z, wall.p2.z);

		aabb_min.x = std::fminf(aabb_min.x, wall.p3.x);
		aabb_min.y = std::fminf(aabb_min.y, wall.p3.y);
		aabb_min.z = std::fminf(aabb_min.z, wall.p3.z);
	}

	/*float f;
	bounds = 0.0f;
	for (auto i=walls.begin();i!=walls.end();++i)
	{
		f = i->p1.LenPwr();
		if (f>bounds)
			bounds = f;
		f = i->p2.LenPwr();
		if (f>bounds)
			bounds = f;
		f = i->p3.LenPwr();
		if (f>bounds)
			bounds = f;
	}
	bounds = sqrt(bounds);*/
}

void CollisionMesh::CalcGrid(void)
{
	Vec3 dif = aabb_max - aabb_min;

	grid_w = max(1, dif.x / 2.0f);
	grid_h = max(1, dif.y / 2.0f);
	grid_d = max(1, dif.z / 2.0f);

	grid.clear();
	grid.resize(grid_w * grid_h * grid_d);

	for (size_t i = 0; i < walls.size(); i++)
	{
		auto aabb = walls[i].GetAABB();
		auto cells = GetCellsInAABB(aabb.first, aabb.second);
		//std::cout << cells.size() << std::endl;
		for each (auto cell in cells)
		{
			grid[cell].push_back(i);
		}
	}
}

std::vector<size_t> CollisionMesh::GetCellsInAABB(Vec3 box_min, Vec3 box_max) const
{
	Vec3 dif = aabb_max - aabb_min;

	Vec3 min_scaled = box_min - aabb_min;
	Vec3 max_scaled = box_max - aabb_min;

	size_t x_min = std::ceilf(min_scaled.x * (grid_w / dif.x)) - 1;
	size_t y_min = std::ceilf(min_scaled.y * (grid_h / dif.y)) - 1;
	size_t z_min = std::ceilf(min_scaled.z * (grid_d / dif.z)) - 1;

	size_t x_max = std::floorf(max_scaled.x * (grid_w / dif.x));
	size_t y_max = std::floorf(max_scaled.y * (grid_h / dif.y));
	size_t z_max = std::floorf(max_scaled.z * (grid_d / dif.z));

	std::vector<size_t> ret;

	for (size_t x = max(0, x_min); x <= min(grid_w - 1, x_max); x++)
	{
		for (size_t y = max(0, y_min); y <= min(grid_h - 1, y_max); y++)
		{
			for (size_t z = max(0, z_min); z <= min(grid_d - 1, z_max); z++)
			{
				ret.push_back(x + y * grid_w + z * grid_w * grid_h);
			}
		}
	}

	return ret;
}

std::vector<size_t> CollisionMesh::GetWallsInAABB(Vec3 box_min, Vec3 box_max) const
{
	if (grid.empty())
	{
		std::vector<size_t> ret;
		ret.resize(walls.size());

		for (size_t i = 0; i < walls.size(); i++)
		{
			ret[i] = i;
		}

		return ret;
	}

	Vec3 dif = aabb_max - aabb_min;

	Vec3 min_scaled = box_min - aabb_min;
	Vec3 max_scaled = box_max - aabb_min;

	size_t x_min = std::ceilf(min_scaled.x * (grid_w / dif.x)) - 1;
	size_t y_min = std::ceilf(min_scaled.y * (grid_h / dif.y)) - 1;
	size_t z_min = std::ceilf(min_scaled.z * (grid_d / dif.z)) - 1;

	size_t x_max = std::floorf(max_scaled.x * (grid_w / dif.x));
	size_t y_max = std::floorf(max_scaled.y * (grid_h / dif.y));
	size_t z_max = std::floorf(max_scaled.z * (grid_d / dif.z));

	std::set<size_t> walls_in_aabb;

	for (size_t x = max(0, x_min); x <= min(grid_w - 1, x_max); x++)
	{
		for (size_t y = max(0, y_min); y <= min(grid_h - 1, y_max); y++)
		{
			for (size_t z = max(0, z_min); z <= min(grid_d - 1, z_max); z++)
			{
				for each (auto wall in grid[x + y * grid_w + z * grid_w * grid_h])
				{
					walls_in_aabb.insert(wall);
				}
			}
		}
	}

	std::vector<size_t> ret;

	for each (auto wall in walls_in_aabb)
	{
		ret.push_back(wall);
	}

	return ret;
}

void CollisionMesh::SphereCheck(const Vec3& vP, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	for (int i=0;i<walls.size();++i)
	{
		std::shared_ptr<Collision> col = walls[i].SphereCheck(vP, r);
		if (col!=0)
			list.push_back(col);
	}
}

void CollisionMesh::LineCheck(const Vec3& sP, const Vec3& eP, std::vector<std::shared_ptr<Collision>>& list)
{
	Vec3 box_min = Vec3(INFINITE, INFINITE, INFINITE);
	Vec3 box_max = -box_min;

	box_max.x = std::fmaxf(box_max.x, sP.x);
	box_max.y = std::fmaxf(box_max.y, sP.y);
	box_max.z = std::fmaxf(box_max.z, sP.z);

	box_max.x = std::fmaxf(box_max.x, eP.x);
	box_max.y = std::fmaxf(box_max.y, eP.y);
	box_max.z = std::fmaxf(box_max.z, eP.z);

	box_min.x = std::fminf(box_min.x, sP.x);
	box_min.y = std::fminf(box_min.y, sP.y);
	box_min.z = std::fminf(box_min.z, sP.z);

	box_min.x = std::fminf(box_min.x, eP.x);
	box_min.y = std::fminf(box_min.y, eP.y);
	box_min.z = std::fminf(box_min.z, eP.z);

	auto walls_in_box = GetWallsInAABB(box_min, box_max);
	//std::cout << walls_in_box.size() << std::endl;
	for each (auto wall in walls_in_box)
	{
		std::shared_ptr<Collision> col = walls[wall].LineCheck(sP, eP);
		if (col != nullptr) {
			col->ce = &walls[wall];
			list.push_back(col);
		}
	}
}

void CollisionMesh::SphereCast(const Vec3& sP, const Vec3& eP, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	Vec3 box_min = Vec3(INFINITE, INFINITE, INFINITE);
	Vec3 box_max = -box_min;

	box_max.x = std::fmaxf(box_max.x, sP.x);
	box_max.y = std::fmaxf(box_max.y, sP.y);
	box_max.z = std::fmaxf(box_max.z, sP.z);

	box_max.x = std::fmaxf(box_max.x, eP.x);
	box_max.y = std::fmaxf(box_max.y, eP.y);
	box_max.z = std::fmaxf(box_max.z, eP.z);

	box_min.x = std::fminf(box_min.x, sP.x);
	box_min.y = std::fminf(box_min.y, sP.y);
	box_min.z = std::fminf(box_min.z, sP.z);

	box_min.x = std::fminf(box_min.x, eP.x);
	box_min.y = std::fminf(box_min.y, eP.y);
	box_min.z = std::fminf(box_min.z, eP.z);

	box_min += Vec3(-r, -r, -r);
	box_max += Vec3(r, r, r);

	auto walls_in_box = GetWallsInAABB(box_min, box_max);
	//std::cout << walls_in_box.size() << std::endl;
	for each (auto wall in walls_in_box)
	{
		std::shared_ptr<Collision> col = walls[wall].SphereCast(sP, eP, r);
		if (col != nullptr) {
			col->ce = &walls[wall];
			list.push_back(col);
		}
	}
}

void CollisionMesh::DiskCast(const Vec3& sP, const Vec3& eP, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	Vec3 box_min = Vec3(INFINITE, INFINITE, INFINITE);
	Vec3 box_max = -box_min;

	box_max.x = std::fmaxf(box_max.x, sP.x);
	box_max.y = std::fmaxf(box_max.y, sP.y);
	box_max.z = std::fmaxf(box_max.z, sP.z);

	box_max.x = std::fmaxf(box_max.x, eP.x);
	box_max.y = std::fmaxf(box_max.y, eP.y);
	box_max.z = std::fmaxf(box_max.z, eP.z);

	box_min.x = std::fminf(box_min.x, sP.x);
	box_min.y = std::fminf(box_min.y, sP.y);
	box_min.z = std::fminf(box_min.z, sP.z);

	box_min.x = std::fminf(box_min.x, eP.x);
	box_min.y = std::fminf(box_min.y, eP.y);
	box_min.z = std::fminf(box_min.z, eP.z);

	box_min += Vec3(-r, -r, -r);
	box_max += Vec3(r, r, r);

	auto walls_in_box = GetWallsInAABB(box_min, box_max);
	//std::cout << walls_in_box.size() << std::endl;
	for each (auto wall in walls_in_box)
	{
		std::shared_ptr<Collision> col = walls[wall].DiskCast(sP, eP, r);
		if (col != nullptr) {
			col->ce = &walls[wall];
			list.push_back(col);
		}
	}
}

void CollisionMesh::LowerDisk(const Vec3 & lock, const Vec3 & center, const Vec3 & axis, const Vec3 & dir, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	for (int i = 0; i<walls.size(); ++i)
	{
		std::shared_ptr<Collision> col = walls[i].LowerDisk(lock, center, axis, dir, r);
		if (col != 0) {
			col->ce = &walls[i];
			list.push_back(col);
		}
	}
}

void CollisionMesh::debug_render(void)
{
	float col[3] = {1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, col);

	glBegin(GL_TRIANGLES);

	for (int i=0;i<walls.size();++i)
	{
		glNormal3f(walls[i].n.x, walls[i].n.y, walls[i].n.z);
		glVertex3f(walls[i].p1.x, walls[i].p1.y, walls[i].p1.z);
		glVertex3f(walls[i].p2.x, walls[i].p2.y, walls[i].p2.z);
		glVertex3f(walls[i].p3.x, walls[i].p3.y, walls[i].p3.z);
	}

	glEnd();

	glBegin(GL_LINES);

	for (int i=0;i<walls.size();++i)
	{
		glNormal3f(walls[i].n.x, walls[i].n.y, walls[i].n.z);
		Vec3 med = (walls[i].p1+walls[i].p2+walls[i].p3)/3.0f;
		glVertex3f(med.x, med.y, med.z);
		med += walls[i].n;
		glVertex3f(med.x, med.y, med.z);
	}

	glEnd();
}