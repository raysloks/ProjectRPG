#include "Mesh.h"
#include "GUIObject.h"
#include "Texture.h"
#include "RenderSetup.h"
#include "MaterialList.h"

Vertex::Vertex(const Vec3& pos) : p(pos) {}

Face::Face(unsigned short va, unsigned short vb, unsigned short vc) : a(va), b(vb), c(vc) {}

VertexStruct::VertexStruct(const std::string& a, size_t s, bool n, size_t st, size_t o) : attribute_name(a), size(s), normalize(n), stride(st), offset(o)
{
}

VertexStruct::~VertexStruct()
{
}

void VertexStruct::refresh(RenderSetup& rs)
{
	attribute_id = rs.current_program->GetAttributeLocation(attribute_name);
}

void VertexStruct::enable()
{
	if (attribute_id != -1)
		glEnableVertexAttribArray(attribute_id);
}

void VertexStruct::specify()
{
	if (attribute_id != -1)
		glVertexAttribPointer(attribute_id, size, GL_FLOAT, normalize, stride, (void*)offset);
}

void VertexStruct::disable()
{
	if (attribute_id != -1)
		glDisableVertexAttribArray(attribute_id);
}

VBO::VBO(void)
{
}

VBO::~VBO()
{
	for (auto i = buffers.begin(); i != buffers.end(); ++i)
		if (i->first != 0)
			glDeleteBuffers(1, &i->first);
}

void VBO::addBuffer()
{
	GLuint buffer_id;
	glGenBuffers(1, &buffer_id);
	buffers.push_back(std::make_pair(buffer_id, std::vector<VertexStruct>()));
}

void VBO::addVertexStruct(const VertexStruct& vs)
{
	buffers.back().second.push_back(vs);
}

void VBO::draw(RenderSetup& rs)
{
	for (auto i = buffers.begin(); i != buffers.end(); ++i)
	{
		for (auto v = i->second.begin(); v != i->second.end(); ++v)
		{
			v->refresh(rs);
		}
	}

	for (auto i = buffers.begin(); i != buffers.end(); ++i)
	{
		for (auto v = i->second.begin(); v != i->second.end(); ++v)
		{
			v->enable();
		}
	}

	for (auto i = buffers.begin(); i != buffers.end(); ++i)
	{
		glBindBuffer(GL_ARRAY_BUFFER, i->first);
		for (auto v = i->second.begin(); v != i->second.end(); ++v)
		{
			v->specify();
		}
	}

	glDrawArrays(GL_TRIANGLES, 0, nIndices);

	for (auto i = buffers.begin(); i != buffers.end(); ++i)
	{
		for (auto v = i->second.begin(); v != i->second.end(); ++v)
		{
			v->disable();
		}
	}
}

Mesh::Mesh(void) : vbo_latest(false)
{
}

Mesh::Mesh(instream& is) : vbo_latest(false)
{
	bool first_mat = true;
	int nt = 0;
	size_t nts = 0;

	sets.push_back(FaceSet());
	char prefix;
	while (!is.eof()) {
		is >> prefix;
		if (prefix == 'v') {
			float x,y,z;
			is >> x >> y >> z;
			vert.push_back(Vec3(x,y,z));
		}
		if (prefix == 'n') {
			float x,y,z;
			is >> x >> y >> z;
			vert.back().n = Vec3(x,y,z);
		}
		if (prefix == 'f') {
			unsigned short a,b,c;
			is >> a >> b >> c;
			sets.back().vertices.push_back(Face(a, b, c));
			nts = 0;
		}
		if (prefix == 'c') {
			std::string name;
			is >> name;
			int l;
			is >> l;
			for (int i=0;i<l;++i)
			{
				float r, g, b;
				is >> r >> g >> b;
			}
		}
		if (prefix == 'i') {
			unsigned short a, b, c;
			is >> a >> b >> c;
		}
		if (prefix == 'u') {
			float x,y;
			is >> x >> y;
			uv.push_back(Vec2(x, y));
		}
		if (prefix == 't') {
			unsigned short i;
			is >> i;
			switch (nt)
			{
			case 0:
				sets.back().uv_points.push_back(Face(i, 0, 0));
				sets.back().nTextures = ++nts;
				nt = 1;
				break;
			case 1:
				sets.back().uv_points.back().b = i;
				nt = 2;
				break;
			case 2:
				sets.back().uv_points.back().c = i;
				nt = 0;
				break;
			}
		}
		if (prefix == 'w') {
			int id;
			float weight;
			is >> id >> weight;
			vert.back().w.insert(std::pair<int, float>(id, weight));
		}
		if (prefix == 'm') {
			if (first_mat) {
				first_mat = false;
			} else {
				sets.push_back(FaceSet());
			}
		}
		if (prefix == 'p') {
			std::string prop;
			char type;
			is >> prop >> type;
			if (type == 's') {
				std::string value;
				is >> value;
			}
			if (type == 'f') {
				float value;
				is >> value;
			}
		}
	}
}

Mesh::Mesh(const Mesh& mesh)
{
	*this = mesh;
	vbos.clear();
	vbo_latest = false;
}

Mesh::~Mesh(void)
{
}

#include "Profiler.h"

void Mesh::render(RenderSetup& rs, MaterialList& mats) // maybe should get mats to be const
{
	if (!vbo_latest)
		buildVBO();

	for (size_t i = 0; i < sets.size(); i++)
	{
		if (i < mats.materials.size())
		{
			mats.materials[i].bindTextures();

			rs.pushMod(mats.materials[i].mod);

			rs.applyMods();

			vbos[i]->draw(rs);

			rs.popMod();
		}
	}
}

void Mesh::transform(const Matrix4& mtrx, Mesh * mesh)
{
	for (int i=0;i<vert.size();++i)
	{
		mesh->vert[i].n = vert[i].n*Matrix3(mtrx);
		mesh->vert[i].p = vert[i].p*mtrx;
	}
	vbo_latest = false;
}

void Mesh::getPose(const Pose& pose, Mesh * mesh)
{
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	if (pose.rest!=0)
	{
		std::vector<Matrix4> dif(pose.bones.size());
		for (int i=0;i<pose.bones.size();++i)
		{
			dif[i] = pose.rest->bones[i].total_inverse*pose.bones[i].total_transform;
		}

		std::map<int, float>::iterator it;
		int size = vert.size();
		for (int i=0;i<size;++i)
		{
			float total_w = 0.0f;
			mesh->vert[i].p = Vec3();
			mesh->vert[i].n = Vec3();
			Vec3 p;
			for (it=mesh->vert[i].w.begin();it!=mesh->vert[i].w.end();++it)
			{
				if (it->second>0.0f)
				{
					if (it->first<pose.rest->bones.size())
					{
						p = vert[i].p*dif[it->first];
						mesh->vert[i].p += p*it->second;

						p = vert[i].n*Matrix3(dif[it->first]);
						mesh->vert[i].n += p*it->second;

						total_w += it->second;
					}
				}
			}
			if (total_w>0.0f) {
				mesh->vert[i].p /= total_w;
				mesh->vert[i].n.Normalize();
			}
		}

		mesh->vbo_latest = false;
	}

	QueryPerformanceCounter(&end);
	double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	Profiler::add("skinning", durationInSeconds);
}

void Mesh::addVBO(size_t set, Vec3 * v_data, Vec3 * n_data, Vec2 * t_data, Matrix4 mtrx)
{
	auto& setref = sets[set];

	for (auto p = setref.vertices.begin(); p != setref.vertices.end(); ++p)
	{
		v_data[(p - setref.vertices.begin()) * 3] = vert[p->a].p * mtrx;
		v_data[(p - setref.vertices.begin()) * 3 + 1] = vert[p->b].p * mtrx;
		v_data[(p - setref.vertices.begin()) * 3 + 2] = vert[p->c].p * mtrx;
	}

	for (auto p = setref.vertices.begin(); p != setref.vertices.end(); ++p)
	{
		n_data[(p - setref.vertices.begin()) * 3] = vert[p->a].n * Matrix3(mtrx);
		n_data[(p - setref.vertices.begin()) * 3 + 1] = vert[p->b].n * Matrix3(mtrx);
		n_data[(p - setref.vertices.begin()) * 3 + 2] = vert[p->c].n * Matrix3(mtrx);
	}

	for (auto p = setref.uv_points.begin(); p != setref.uv_points.end(); ++p)
	{
		t_data[(p - setref.uv_points.begin()) * 3] = uv[p->a];
		t_data[(p - setref.uv_points.begin()) * 3 + 1] = uv[p->b];
		t_data[(p - setref.uv_points.begin()) * 3 + 2] = uv[p->c];
	}
}

void Mesh::buildVBO(void)
{
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	if (sets.size()>vbos.size())
		vbos.resize(sets.size());
	for (auto i=vbos.begin();i!=vbos.end();++i)
	{
		auto& setref = sets[i-vbos.begin()];

		if (*i == nullptr)
		{
			i->reset(new VBO());

			(*i)->nIndices = setref.vertices.size() * 3;

			(*i)->addBuffer();
			(*i)->addVertexStruct(VertexStruct("pos", 3, false, 0, 0));

			(*i)->addBuffer();
			(*i)->addVertexStruct(VertexStruct("normal", 3, false, 0, 0));

			if (setref.nTextures > 0)
			{
				(*i)->addBuffer();
				for (size_t j = 0; j < setref.nTextures; j++)
				{
					(*i)->addVertexStruct(VertexStruct(std::string("texCoord") + std::to_string(j), 2, false, 8 * setref.nTextures, 8 * j));
				}
			}
		}

		//generate data
		Vec3 * v_data = new Vec3[setref.vertices.size() * 3];
		for (auto p=setref.vertices.begin();p!=setref.vertices.end();++p)
		{
			v_data[(p-setref.vertices.begin())*3] = vert[p->a].p;
			v_data[(p-setref.vertices.begin())*3+1] = vert[p->b].p;
			v_data[(p-setref.vertices.begin())*3+2] = vert[p->c].p;
		}

		Vec3 * n_data = new Vec3[setref.vertices.size() * 3];
		for (auto p=setref.vertices.begin();p!=setref.vertices.end();++p)
		{
			n_data[(p-setref.vertices.begin())*3] = vert[p->a].n;
			n_data[(p-setref.vertices.begin())*3+1] = vert[p->b].n;
			n_data[(p-setref.vertices.begin())*3+2] = vert[p->c].n;
		}

		Vec2 * t_data = new Vec2[setref.uv_points.size() * 3];
		for (auto p=setref.uv_points.begin();p!=setref.uv_points.end();++p)
		{
			t_data[(p-setref.uv_points.begin())*3] = uv[p->a];
			t_data[(p-setref.uv_points.begin())*3+1] = uv[p->b];
			t_data[(p-setref.uv_points.begin())*3+2] = uv[p->c];
		}

		// push buffer data
		glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[0].first);
		glBufferData(GL_ARRAY_BUFFER, setref.vertices.size() * 3 * sizeof(Vec3), v_data, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[1].first);
		glBufferData(GL_ARRAY_BUFFER, setref.vertices.size() * 3 * sizeof(Vec3), n_data, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[2].first);
		glBufferData(GL_ARRAY_BUFFER, setref.uv_points.size() * 3 * sizeof(Vec2), t_data, GL_STATIC_DRAW);

		delete [] v_data;
		delete [] n_data;
		delete [] t_data;

		vbo_latest = true;
	}

	QueryPerformanceCounter(&end);
	double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	Profiler::add("vbo-build", durationInSeconds);
}