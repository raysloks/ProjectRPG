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
			for (auto it = set->begin();it!=set->end();++it)
			{
				walls.push_back(DynamicWall(pMesh->vert[it->a].p,pMesh->vert[it->b].p,pMesh->vert[it->c].p));
			}
		}
		CalcBounds();
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
			for (auto it = set->begin();it!=set->end();++it)
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
			for (auto it = set->begin();it!=set->end();++it)
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
}

void CollisionMesh::Recalculate(const Mesh& mesh, const Mesh& next_mesh, const Matrix4& mtrx, const Matrix4& next_mtrx, float dTime)
{
	if (walls.size()==0)
	{
		for (auto set = mesh.sets.begin();set!=mesh.sets.end();++set)
		{
			for (auto it = set->begin();it!=set->end();++it)
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
			for (auto it = set->begin();it!=set->end();++it)
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
}

void CollisionMesh::Recalculate(void)
{
	float f;
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
	bounds = sqrt(bounds);
}

void CollisionMesh::CalcBounds(void)
{
	float f;
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
	bounds = sqrt(bounds);
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
	for (int i=0;i<walls.size();++i)
	{
		std::shared_ptr<Collision> col = walls[i].LineCheck(sP, eP);
		if (col!=0)
			list.push_back(col);
	}
}

void CollisionMesh::SphereCast(const Vec3& sP, const Vec3& eP, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	float f = ((sP+eP)/2.0f).Len()-(eP-sP).Len()/2.0f-r;
	if (f<=bounds)
	{
		for (int i=0;i<walls.size();++i)
		{
			f = ((sP+eP)/2.0f-walls[i].p1).Len()-(eP-sP).Len()/2.0f-r;
			if (walls[i].l12>=f || walls[i].l23>=f || walls[i].l31>=f)
			{
				std::shared_ptr<Collision> col = walls[i].SphereCast(sP, eP, r);
				if (col!=0) {
					col->ce = &walls[i];
					list.push_back(col);
				}
			}
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