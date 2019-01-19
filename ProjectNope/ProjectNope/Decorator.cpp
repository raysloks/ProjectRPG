#include "Decorator.h"
#include "Mesh.h"
#include "Texture.h"

Decorator::Decorator(void)
{
}

Decorator::Decorator(const std::string& m, int bid) : mesh_fname(m), bone_id(bid), skin(0)
{
	Resource::load(mesh_fname);
}

Decorator::Decorator(const std::string& m, const Material& mat, int bid) : mesh_fname(m), bone_id(bid), skin(0)
{
	materials.materials.push_back(mat);
	Resource::load(mesh_fname);
}

Decorator::Decorator(const std::string& m, const MaterialList& mats, int bid) : mesh_fname(m), materials(mats), bone_id(bid), skin(0)
{
	Resource::load(mesh_fname);
}

Decorator::~Decorator(void)
{
}

void Decorator::writeLog(outstream& os)
{
	os << mesh_fname << materials << bone_id;
	if (bone_id>=0)
		os << local;
	os << priority;
}

void Decorator::readLog(instream& is)
{
	is >> mesh_fname >> materials >> bone_id;
	if (bone_id>=0)
		is >> local;
	is >> priority;
}

void Decorator::attach(const Pose& pose)
{
	if (mesh==0)
		mesh = Resource::get<Mesh>(mesh_fname);
	if (mesh!=0)
	{
		if (bone_id>=0)
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
	if (bone_id>=0)
	{
		if (pose.bones.size()>bone_id)
			final = pose.bones[bone_id].getTransform();
	}
}

void Decorator::attach()
{
	if (mesh==0)
		mesh = Resource::get<Mesh>(mesh_fname);
	if (mesh!=0)
	{
		if (bone_id>=0)
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
}

#include "RenderSetup.h"

void Decorator::render(RenderSetup& rs)
{
	if (bone_id>=0) {
		rs.pushTransform();

		rs.addTransform(local * final);

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

void Decorator::render(RenderSetup& rs, const Pose& pose)
{
	rs.pushTransform();

	rs.addTransform(local * final);

	if (mesh != 0)
		mesh->render_skinned(rs, materials, pose);

	rs.popTransform();
}