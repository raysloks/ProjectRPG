#include "AnimatedCollisionMesh.h"
#include "Resource.h"
#include "GUIObject.h"
#include "Texture.h"

#include "World.h"

const AutoSerialFactory<AnimatedCollisionMesh> AnimatedCollisionMesh::_factory_AnimatedCollisionMeshUnit("AnimatedCollisionMesh");

AnimatedCollisionMesh::AnimatedCollisionMesh(const GlobalPosition& pos, const Vec3& vel,
	const std::string& mesh, const std::string& animation, const std::string& texture, const Matrix3& matrix)
	: StaticProp(pos, mesh, texture, matrix), Serializable(_factory_AnimatedCollisionMeshUnit.id), _anim(animation), v(vel)
{
	Resource::load(_anim);
	apl.reset(new APLList());
	skin = 0;
	next_skin = 0;
}

AnimatedCollisionMesh::AnimatedCollisionMesh(instream& is, bool full) : StaticProp(is, full), Serializable(_factory_AnimatedCollisionMeshUnit.id)
{
	pose.reset(new Pose());
	is >> _anim >> v >> *pose;
	Resource::load(_anim);
	apl.reset(new APLList());
	skin = 0;
	next_skin = 0;
}

AnimatedCollisionMesh::AnimatedCollisionMesh(const AnimatedCollisionMesh& other) : StaticProp(other), Serializable(_factory_AnimatedCollisionMeshUnit.id)
{
	pose.reset(new Pose(*other.pose));
	_anim = other._anim;
	v = other.v;
}

AnimatedCollisionMesh::~AnimatedCollisionMesh(void)
{
}

void AnimatedCollisionMesh::writeLog(outstream& os)
{
	StaticProp::writeLog(os);
	os << v;
	if (pose==0)
		os << Pose();
	else
		os << *pose;
}

void AnimatedCollisionMesh::readLog(instream& is)
{
	StaticProp::readLog(is);
	is >> v >> *pose;
	std::shared_ptr<SkeletalAnimation> anim = Resource::get<SkeletalAnimation>(_anim);
	if (anim!=0)
	{
		pose->rest = &anim->armature;
	}
}

void AnimatedCollisionMesh::tick(float dTime)
{
	set_changed();
	std::shared_ptr<SkeletalAnimation> anim = Resource::get<SkeletalAnimation>(_anim);
	if (world->authority)
	{
		if (anim!=0)
		{
			if (pose==0) {
				pose.reset(new Pose(anim->armature));
				pose->rest = &anim->armature;
			}
		}
		if (pose!=0 && anim!=0)
		{
			if (pose->rest!=0)
				apl->tick(dTime, pose);
		}
	}
	std::shared_ptr<Mesh> mesh = Resource::get<Mesh>(_mesh);
	if (mesh!=0)
	{
		if (skin==0) {
			skin=new Mesh(*mesh);
			next_skin=new Mesh(*skin);
		}
	}
	if (skin!=0 && pose!=0 && next_skin!=0 && anim!=0)
	{
		if (pose->rest!=0)
		{
			*skin = Mesh(*next_skin);
			mesh->getPose(*pose, next_skin);
			for (int i=0;i<next_skin->vert.size();++i)
			{
				next_skin->vert[i].p *= _mtrx;
			}
			if (walls.size()==0)
			{
				for (auto set = skin->sets.begin();set!=skin->sets.end();++set)
				{
					for (auto it = set->begin();it!=set->end();++it)
					{
						walls.push_back(DynamicWall(skin->vert[it->a].p,skin->vert[it->b].p,skin->vert[it->c].p));
						walls.back().vV1 = (next_skin->vert[it->a].p-skin->vert[it->a].p)/dTime + v;
						walls.back().vV2 = (next_skin->vert[it->b].p-skin->vert[it->b].p)/dTime + v;
						walls.back().vV3 = (next_skin->vert[it->c].p-skin->vert[it->c].p)/dTime + v;
					}
				}
			}
			else
			{
				auto wall = walls.begin();
				for (auto set = skin->sets.begin();set!=skin->sets.end();++set)
				{
					for (auto it = set->begin();it!=set->end();++it)
					{
						wall->p1 = skin->vert[it->a].p;
						wall->p2 = skin->vert[it->b].p;
						wall->p3 = skin->vert[it->c].p;
						wall->Recalculate();
						wall->vV1 = (next_skin->vert[it->a].p-skin->vert[it->a].p)/dTime + v;
						wall->vV2 = (next_skin->vert[it->b].p-skin->vert[it->b].p)/dTime + v;
						wall->vV3 = (next_skin->vert[it->c].p-skin->vert[it->c].p)/dTime + v;
						++wall;
					}
				}
			}
			CalcBounds();
		}
	}
}

void AnimatedCollisionMesh::render(const GlobalPosition& origin)
{
	/*SkeletalAnimation* anim = Resource::get<SkeletalAnimation>(_anim);
	Mesh* mesh = Resource::get<Mesh>(_mesh);
	if (mesh!=0)
	{
		if (skin==0) {
			skin=new Mesh(*mesh);
		}
	}
	if (pose!=0 && skin!=0)
	{
		mesh->getPose(*pose, skin);
		for (int i=0;i<skin->vert.size();++i)
		{
			skin->vert[i].p *= _mtrx;
		}
		walls.clear();
		for (std::vector<Face>::iterator it=skin->face.begin();it!=skin->face.end();++it)
		{
			walls.push_back(DynamicWall(skin->vert[it->a].p,skin->vert[it->b].p,skin->vert[it->c].p));
		}
	}*/

	glPushMatrix();

	Vec3 pos = p-origin;
	glTranslatef(pos.x, pos.y, pos.z);

	/*float mat[16] = {_mtrx.data[0], _mtrx.data[1], _mtrx.data[2], 0.0f,
		_mtrx.data[3], _mtrx.data[4], _mtrx.data[5], 0.0f,
		_mtrx.data[6], _mtrx.data[7], _mtrx.data[8], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f};
	glMultMatrixf(mat);*/

	float col[3] = {1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, col);
	
	if (skin!=0) {
		for (int i=0;i<_tex.size();++i) {
			std::shared_ptr<Texture> tex = Resource::get<Texture>(_tex[i]);
			skin->slotTexture(i, tex);
		}
		skin->render();
	}

	glPopMatrix();
}

void AnimatedCollisionMesh::write_to(outstream& os, bool full) const
{
	StaticProp::write_to(os, full);
	os << _anim << v;
	if (pose==0)
		os << Pose();
	else
		os << *pose;
}

void AnimatedCollisionMesh::interpolate(Entity * pEntity, float fWeight)
{
	StaticProp::interpolate(pEntity, fWeight);
	AnimatedCollisionMesh * pACMU = dynamic_cast<AnimatedCollisionMesh*>(pEntity);
	if (pACMU!=0)
	{
		if (pose->bones.size()!=pACMU->pose->bones.size()) {
			pose.reset(new Pose(*pACMU->pose));
		}
		pose->interpolate(*pACMU->pose, fWeight);
		pose->rest = pACMU->pose->rest;
	}
}