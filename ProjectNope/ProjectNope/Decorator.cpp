#include "Decorator.h"
#include "Mesh.h"
#include "Texture.h"

Decorator::Decorator(void)
{
}

Decorator::Decorator(const std::string& m, const std::string& texture, int bone_id) : _mesh(m), bid(bone_id), skin(0)
{
	_tex.push_back(texture);
}

Decorator::Decorator(const std::string& m, const std::vector<std::string>& textures, int bone_id) : _mesh(m), _tex(textures), bid(bone_id), skin(0)
{
}

Decorator::~Decorator(void)
{
}

void Decorator::writeLog(outstream& os)
{
	os << _mesh;
	unsigned char tsize = _tex.size();
	os.write((char*)&tsize, 1); // number of textures will probably not be larger than 255
	for (auto i=_tex.begin();i!=_tex.end();++i)
		os << *i;
	os << bid;
	if (bid>=0)
		os << local;
	os << priority;
}

void Decorator::readLog(instream& is)
{
	unsigned char tsize;
	is >> _mesh >> tsize;
	_tex.resize(tsize);
	for (int i=0;i<tsize;++i) {
		is >> _tex[i];
	}
	is >> bid;
	if (bid>=0)
		is >> local;
	is >> priority;
}

void Decorator::attach(const Pose& pose)
{
	if (mesh==0)
		mesh = Resource::get<Mesh>(_mesh);
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
		mesh = Resource::get<Mesh>(_mesh);
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
			mesh = Resource::get<Mesh>(_mesh);
		if (mesh!=0) {
			for (int i=0;i<_tex.size();++i) {
				std::shared_ptr<Texture> tex = Resource::get<Texture>(_tex[i]);
				mesh->slotTexture(i, tex);
			}
			mesh->render(rs);
		}

		rs.popTransform();
	} else {
		if (skin!=0)
		{
			for (int i=0;i<_tex.size();++i) {
				std::shared_ptr<Texture> tex = Resource::get<Texture>(_tex[i]);
				skin->slotTexture(i, tex);
			}
			skin->render(rs);
		}
		else
		{
			if (mesh==0)
				mesh = Resource::get<Mesh>(_mesh);
			if (mesh!=0) {
				for (int i=0;i<_tex.size();++i) {
					std::shared_ptr<Texture> tex = Resource::get<Texture>(_tex[i]);
					mesh->slotTexture(i, tex);
				}
				mesh->render(rs);
			}
		}
	}
}