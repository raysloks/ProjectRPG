#include "Decorator.h"
#include "Mesh.h"
#include "Texture.h"

Decorator::Decorator(void)
{
}

Decorator::Decorator(const std::string& m, const Material& mat, int bone_id) : mesh_fname(m), bid(bone_id), skin(0)
{
	materials.materials.push_back(mat);
	Resource::load(mesh_fname);
}

Decorator::Decorator(const std::string& m, const MaterialList& mats, int bone_id) : mesh_fname(m), materials(mats), bid(bone_id), skin(0)
{
	Resource::load(mesh_fname);
}

Decorator::~Decorator(void)
{
}

void Decorator::writeLog(outstream& os)
{
	os << mesh_fname << materials << bid;
	if (bid>=0)
		os << local;
	os << priority;
}

void Decorator::readLog(instream& is)
{
	unsigned char tsize;
	is >> mesh_fname >> materials >> bid;
	if (bid>=0)
		is >> local;
	is >> priority;
}

void Decorator::attach(const Pose& pose)
{
	if (mesh==0)
		mesh = Resource::get<Mesh>(mesh_fname);
	if (mesh!=0)
	{
		if (bid>=0)
		{
			if (skin!=mesh)
				skin = mesh;
		}
		else
		{
			if (skin==0 || skin==mesh)
				skin.reset(new Mesh(*mesh));
			mesh->getPose(pose, skin.get());
		}
	}
	if (bid>=0)
	{
		if (pose.bones.size()>bid)
			final = local * pose.bones[bid].getTransform();
		else
			final = local;
	}
}

void Decorator::attach()
{
	if (mesh==0)
		mesh = Resource::get<Mesh>(mesh_fname);
	if (mesh!=0)
	{
		if (bid>=0)
		{
			if (skin!=mesh)
				skin = mesh;
		}
		else
		{
			if (skin==0 || skin==mesh)
				skin.reset(new Mesh(*mesh));
		}
	}
	if (bid>=0)
		final = local;
}

#include "RenderSetup.h"

void Decorator::render(RenderSetup& rs)
{
	if (bid>=0) {
		rs.pushTransform();

		rs.addTransform(final);

		if (mesh==0)
			mesh = Resource::get<Mesh>(mesh_fname);
		if (mesh!=0)
			mesh->render(rs, materials);

		rs.popTransform();
	} else {
		if (skin!=0)
			skin->render(rs, materials);
		else
		{
			if (mesh == 0)
				mesh = Resource::get<Mesh>(mesh_fname);
			if (mesh != 0)
				mesh->render(rs, materials);
		}
	}
}