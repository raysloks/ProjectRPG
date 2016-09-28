#include "StaticProp.h"
#include "GUIObject.h"
#include "Resource.h"
#include "Mesh.h"
#include "Texture.h"

#include <iostream>

const AutoSerialFactory<StaticProp> StaticProp::_factory_StaticProp("StaticProp");

StaticProp::StaticProp(const GlobalPosition& pos, const std::string& mesh, const std::string& texture, const Matrix3& matrix) : PNEntity(pos), Serializable(_factory_StaticProp.id)
{
	_mesh = mesh;
	_tex.push_back(texture);
	_mtrx = matrix;
	Resource::load(texture);
	Resource::load(_mesh);

	collision = true;
	camera_collision = true;

	reconstructing = true;
}

StaticProp::StaticProp(const GlobalPosition& pos, const std::string& mesh, const std::vector<std::string>& textures, const Matrix3& matrix) : PNEntity(pos), Serializable(_factory_StaticProp.id)
{
	_mesh = mesh;
	_tex = textures;
	_mtrx = matrix;
	for (auto i=_tex.begin();i!=_tex.end();++i)
		Resource::load(*i);
	Resource::load(_mesh);

	collision = true;
	camera_collision = true;

	reconstructing = true;
}

StaticProp::StaticProp(instream& is, bool full) : PNEntity(is, full), Serializable(_factory_StaticProp.id)
{
	size_t l;
	std::string str;
	is >> _mesh >> l;
	_tex.clear();
	for (int i=0;i<l;++i) {
		is >> str;
		_tex.push_back(str);
		Resource::load(str);
	}
	is >> _mtrx;

	collision = false;
	if (full)
		is >> collision;
	is >> camera_collision;
	Reconstruct();
}

StaticProp::~StaticProp(void)
{
}

void StaticProp::writeLog(outstream& os)
{
	PNEntity::writeLog(os);
	os << _mtrx;
}

void StaticProp::readLog(instream& is)
{
	PNEntity::readLog(is);
	is >> _mtrx;
	/*for (int i=0;i<3;++i) {
		for (int j=0;j<3;++j)
			std::cout << _mtrx.mtrx[i][j] << " ";
		std::cout << std::endl;
	}*/
}

void StaticProp::tick(float dTime)
{
	if (reconstructing)
		Reconstruct();
	if (mesh==0)
		mesh = Resource::get<Mesh>(_mesh);
	tex.resize(_tex.size());
	for (int i=0;i<_tex.size();++i) {
		if (tex[i]==0)
			tex[i] = Resource::get<Texture>(_tex[i]);
	}
}

#include "Profiler.h"

void StaticProp::render(const GlobalPosition& origin)
{
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	glPushMatrix();

	Vec3 pos = p-origin;
	glTranslatef(pos.x, pos.y, pos.z);

	float mat[16] = {_mtrx.data[0], _mtrx.data[1], _mtrx.data[2], 0.0f,
		_mtrx.data[3], _mtrx.data[4], _mtrx.data[5], 0.0f,
		_mtrx.data[6], _mtrx.data[7], _mtrx.data[8], 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f};
	glMultMatrixf(mat);

	float col[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_DIFFUSE, col);

	if (mesh!=0) {
		for (int i=0;i<tex.size();++i)
			mesh->slotTexture(i, tex[i]);
		mesh->render();
	}

	glPopMatrix();

	QueryPerformanceCounter(&end);
	double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	Profiler::add("static-prop-render", durationInSeconds);
}

void StaticProp::write_to(outstream& os, bool full)const
{
	PNEntity::write_to(os, full);
	os << _mesh << _tex.size();
	for (int i=0;i<_tex.size();++i)
		os << _tex[i];
	os << _mtrx;
	if (full)
		os << collision;
	os << camera_collision;
}

void StaticProp::interpolate(Entity * pEntity, float fWeight)
{
	PNEntity::interpolate(pEntity, fWeight);
	StaticProp * pProp = dynamic_cast<StaticProp*>(pEntity);
	if (pProp!=0) {
		_mtrx = pProp->_mtrx;//(_mtrx*(1.0f-fWeight)+pProp->_mtrx*fWeight);
		collision = pProp->collision;
		camera_collision = pProp->camera_collision;
	}
	if (changed)
		Reconstruct();
}


void StaticProp::Reconstruct(void)
{
	if (collision || camera_collision)
	{
		reconstructing = true;
		if (LoadFromResource(_mesh)) {
			for (int i=0;i<walls.size();++i)
			{
				walls[i].p1 = _mtrx * walls[i].p1;
				walls[i].p2 = _mtrx * walls[i].p2;
				walls[i].p3 = _mtrx * walls[i].p3;
				walls[i].Recalculate();
			}
			CalcBounds();
			reconstructing = false;
		}
	}
	else
	{
		reconstructing = false;
	}
}