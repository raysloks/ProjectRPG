#include "Mesh.h"

#include <algorithm>

#include "GUIObject.h"
#include "Texture.h"
#include "RenderSetup.h"
#include "MaterialList.h"

#include "Profiler.h"

Vertex::Vertex(const Vec3& pos) : p(pos) {}

Face::Face(uint32_t va, uint32_t vb, uint32_t vc) : a(va), b(vb), c(vc) {}

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
	TimeslotC(vbo_destructor);
	for (auto i = buffers.begin(); i != buffers.end(); ++i)
		if (i->first != 0)
			Platform::RenderContext::ReleaseBuffer(i->first);
	for each (auto vao in vaos)
		Platform::RenderContext::ReleaseVertexArray(vao.second);
}

void VBO::addBuffer()
{
	TimeslotC(vbo_add_buffer);
	GLuint buffer_id = gRenderContext->GetBuffer();
	buffers.push_back(std::make_pair(buffer_id, std::vector<VertexStruct>()));
}

void VBO::addVertexStruct(const VertexStruct& vs)
{
	buffers.back().second.push_back(vs);
}

void VBO::bind(RenderSetup& rs)
{
	TimeslotC(vbo_bind);

	auto vao_it = vaos.find(rs.current_program->gl_program);
	if (vao_it == vaos.end())
	{
		GLuint vao = gRenderContext->GetVertexArray();
		TimeslotC(vbo_bind_bind);
		glBindVertexArray(vao);
		
		TimeslotC(vbo_bind_refresh_and_stuff);

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

		vaos.insert(std::make_pair(rs.current_program->gl_program, vao));
	}
	else
	{
		TimeslotC(vbo_bind_existing);
		glBindVertexArray(vao_it->second);
	}
}

void VBO::draw(RenderSetup& rs)
{
	TimeslotC(vbo_draw);

	bind(rs);

	glDrawArrays(GL_TRIANGLES, 0, nIndices);

	glBindVertexArray(0);
}

void VBO::draw_instanced(RenderSetup & rs, unsigned int nInstances)
{
	TimeslotC(vbo_draw_instanced);

	bind(rs);

	glDrawArraysInstanced(GL_TRIANGLES, 0, nIndices, nInstances);

	glBindVertexArray(0);
}

Mesh::Mesh(void) : vbo_latest(false)
{
}

Mesh::Mesh(instream& is, const std::string& path) : vbo_latest(false)
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
			uint16_t a,b,c;
			is >> a >> b >> c;
			sets.back().vertices.push_back(Face(a, b, c));
			nts = 0;
		}
		if (prefix == 'F') {
			uint32_t a, b, c;
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
		if (prefix == 'I') {
			uint32_t a, b, c;
			is >> a >> b >> c;
		}
		if (prefix == 'u') {
			float x,y;
			is >> x >> y;
			uv.push_back(Vec2(x, y));
		}
		if (prefix == 't') {
			uint16_t i;
			is >> i;
			switch (nt)
			{
			case 0:
				sets.back().uv_points.push_back(Face(i, 0, 0));
				sets.back().nTextures = nts + 1;
				nt = 1;
				break;
			case 1:
				sets.back().uv_points.back().b = i;
				nt = 2;
				break;
			case 2:
				sets.back().uv_points.back().c = i;
				nt = 0;
				++nts;
				break;
			}
		}
		if (prefix == 'T') {
			uint32_t i;
			is >> i;
			switch (nt)
			{
			case 0:
				sets.back().uv_points.push_back(Face(i, 0, 0));
				sets.back().nTextures = nts + 1;
				nt = 1;
				break;
			case 1:
				sets.back().uv_points.back().b = i;
				nt = 2;
				break;
			case 2:
				sets.back().uv_points.back().c = i;
				nt = 0;
				++nts;
				break;
			}
		}
		if (prefix == 'w') {
			uint32_t id;
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
		if (prefix == 'M')
		{
			std::string tex;
			is >> tex;
			tex = path + tex;
			sets.back().default_material.tex.push_back(tex);
		}
		if (prefix == 'o')
		{
			std::string option;
			is >> option;
			sets.back().default_material.tex.back().options.push_back(option);
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

void Mesh::subdivide()
{
	int** edge = new int*[vert.size()];
	for (int i = 0; i<vert.size(); i++) {
		edge[i] = new int[i];
		for (int j = 0; j<i; j++)
			edge[i][j] = -1;
	}

	int	*e1, *e2, *e3;
	for (auto setr = sets.begin(); setr != sets.end(); ++setr)
	{
		auto& set = setr->vertices;
		int startsize = set.size();
		for (int i = 0; i<startsize; ++i) {
			if (set[i].a>set[i].b)
				e1 = &edge[set[i].a][set[i].b];
			else
				e1 = &edge[set[i].b][set[i].a];
			if (*e1 == -1) {
				*e1 = vert.size();
				vert.push_back(Vertex((vert[set[i].a].p + vert[set[i].b].p) / 2/*, (vert[set[i].a].c + vert[set[i].b].c) / 2*/));
			}
			if (set[i].b>set[i].c)
				e2 = &edge[set[i].b][set[i].c];
			else
				e2 = &edge[set[i].c][set[i].b];
			if (*e2 == -1) {
				*e2 = vert.size();
				vert.push_back(Vertex((vert[set[i].b].p + vert[set[i].c].p) / 2/*, (vert[set[i].b].c + vert[set[i].c].c) / 2*/));
			}
			if (set[i].c>set[i].a)
				e3 = &edge[set[i].c][set[i].a];
			else
				e3 = &edge[set[i].a][set[i].c];
			if (*e3 == -1) {
				*e3 = vert.size();
				vert.push_back(Vertex((vert[set[i].c].p + vert[set[i].a].p) / 2/*, (vert[set[i].c].c + vert[set[i].a].c) / 2*/));
			}

			set.push_back(Face(*e1, set[i].b, *e2));
			set.push_back(Face(*e2, set[i].c, *e3));
			set.push_back(Face(*e3, set[i].a, *e1));
			set[i] = Face(*e1, *e2, *e3);
		}
	}
}

void Mesh::toSphere(float radius)
{
	for (int i = 0; i<vert.size(); i++) {
		vert[i].p /= vert[i].p.Len();
		vert[i].n = vert[i].p;
		vert[i].p *= radius;
	}
}

void Mesh::genUV()
{
}

void Mesh::render(RenderSetup& rs, MaterialList& mats) // maybe should get mats to be const
{
	TimeslotC(mesh_render);

	if (!vbo_latest)
		buildVBO();

	for (size_t i = 0; i < sets.size(); i++)
	{
		auto& mat = sets[i].default_material;
		if (i < mats.materials.size())
			mat = mats.materials[i];

		mat.bindTextures();

		TimeslotC(mod_draw);

		rs.pushMod(mat.mod);

		rs.applyMods();

		vbos[i]->draw(rs);

		rs.popMod();
	}
}

void Mesh::render_skinned(RenderSetup& rs, MaterialList& mats, const Pose& pose)
{
	TimeslotC(mesh_render);

	auto transforms = pose.transforms;

	if (!vbo_latest)
		buildVBO();

	for (size_t i = 0; i < sets.size(); i++)
	{
		auto& mat = sets[i].default_material;
		if (i < mats.materials.size())
			mat = mats.materials[i];

		mat.bindTextures();

		rs.pushMod(mat.mod);

		rs.applyMods();
		auto shader = ShaderProgram::Get(Shader::get("data/gfill_skinned_vert.txt", SHADER_VERTEX), rs.current_program->geom, rs.current_program->frag);

		ShaderMod mod(shader, [=](const std::shared_ptr<ShaderProgram>& prog) {
			prog->UniformMatrix4fv("bone_transform", transforms);
		});

		rs.pushMod(mod);
		rs.applyMods();

		vbos[i]->draw(rs);

		rs.popMod();
		rs.popMod();
	}
}

void Mesh::render_instanced(RenderSetup& rs, MaterialList& mats, unsigned int nInstances)
{
	TimeslotC(mesh_render_instanced);

	if (!vbo_latest)
		buildVBO();

	for (size_t i = 0; i < sets.size(); i++)
	{
		auto& mat = sets[i].default_material;
		if (i < mats.materials.size())
			mat = mats.materials[i];

		mat.bindTextures();

		rs.pushMod(mat.mod);

		rs.applyMods();

		vbos[i]->draw_instanced(rs, nInstances);

		rs.popMod();
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
	TimeslotC(skinning);

	if (pose.rest != nullptr)
	{
		std::vector<Matrix4> dif(pose.bones.size());
		for (size_t i = 0; i < pose.bones.size(); i++)
		{
			dif[i] = pose.rest->bones[i].total_inverse*pose.bones[i].total_transform;
		}

		int size = vert.size();
		for (int i=0;i<size;++i)
		{
			float total_w = 0.0f;
			mesh->vert[i].p = Vec3();
			mesh->vert[i].n = Vec3();
			Vec3 p;
			for (auto it = mesh->vert[i].w.begin(); it != mesh->vert[i].w.end(); ++it)
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
	TimeslotB(buildVBO);

	if (sets.size()>vbos.size())
		vbos.resize(sets.size());

	for (auto i = vbos.begin(); i != vbos.end(); ++i)
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

			if (!vert.empty())
			{
				if (!vert.front().w.empty())
				{
					(*i)->addBuffer();
					(*i)->addVertexStruct(VertexStruct("bone_weight", 4, false, 0, 0));
					(*i)->addBuffer();
					(*i)->addVertexStruct(VertexStruct("bone_index", 4, false, 0, 0));
				}
			}
		}

		// generate data
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
		size_t block = 0;
		for (auto p=setref.uv_points.begin();p!=setref.uv_points.end();)
		{
			for (size_t j = 0; j < setref.nTextures; j++)
			{
				t_data[block + j] = uv[p->a];
				t_data[block + j + setref.nTextures] = uv[p->b];
				t_data[block + j + setref.nTextures * 2] = uv[p->c];
				++p;
			}
			block += 3 * setref.nTextures;
		}

		float * w_data = new float[setref.vertices.size() * 3 * 4];
		float * i_data = new float[setref.vertices.size() * 3 * 4];
		for (size_t i = 0; i < setref.vertices.size(); i++)
		{
			for (size_t j = 0; j < 3; j++)
			{
				unsigned int vertex_id;
				switch (j)
				{
				case 0:
					vertex_id =	setref.vertices[i].a;
					break;
				case 1:
					vertex_id = setref.vertices[i].b;
					break;
				case 2:
					vertex_id = setref.vertices[i].c;
				}

				std::vector<std::pair<int, float>> weights;
				for each (auto weight in vert[vertex_id].w)
				{
					weights.push_back(weight);
				}
				std::stable_sort(weights.begin(), weights.end(), [](auto lhs, auto rhs)
				{
					return lhs.second > rhs.second;
				});
				if (weights.size() > 4)
				{
					weights.resize(4);
				}

				for (size_t k = 0; k < 4; k++)
				{
					if (k < weights.size())
					{
						w_data[i * 3 * 4 + j * 4 + k] = weights[k].second;
						i_data[i * 3 * 4 + j * 4 + k] = weights[k].first;
					}
					else
					{
						w_data[i * 3 * 4 + j * 4 + k] = 0.0f;
						i_data[i * 3 * 4 + j * 4 + k] = 0.0f;
					}
				}
			}
		}

		// push buffer data
		glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[0].first);
		glBufferData(GL_ARRAY_BUFFER, setref.vertices.size() * 3 * sizeof(Vec3), v_data, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[1].first);
		glBufferData(GL_ARRAY_BUFFER, setref.vertices.size() * 3 * sizeof(Vec3), n_data, GL_STATIC_DRAW);

		if (setref.nTextures > 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[2].first);
			glBufferData(GL_ARRAY_BUFFER, setref.uv_points.size() * 3 * sizeof(Vec2), t_data, GL_STATIC_DRAW);
		}

		if (!vert.empty())
		{
			if (!vert.front().w.empty())
			{
				size_t buf = (setref.nTextures > 0) ? 3 : 2;
				glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[buf].first);
				glBufferData(GL_ARRAY_BUFFER, setref.vertices.size() * 3 * 4 * sizeof(float), w_data, GL_STATIC_DRAW);
				glBindBuffer(GL_ARRAY_BUFFER, (*i)->buffers[buf + 1].first);
				glBufferData(GL_ARRAY_BUFFER, setref.vertices.size() * 3 * 4 * sizeof(float), i_data, GL_STATIC_DRAW);
			}
		}

		delete[] i_data;
		delete[] w_data;
		delete[] t_data;
		delete[] v_data;
		delete[] n_data;

		vbo_latest = true;
	}
}

void Mesh::bind(const Pose& pose)
{
	//for each (auto& v in vert)
	for (auto& v : vert)
	{
		size_t closest = 0;
		float closest_distance = 1000.0f;
		for (size_t i = 0; i < pose.bones.size(); i++)
		{
			Vec3 bone_pos = Vec3() * pose.bones[i].total_transform;
			float distance = (bone_pos - v.p).LenPwr();
			if (distance < closest_distance)
			{
				closest = i;
				closest_distance = distance;
			}
		}
		v.w[closest] = 1.0f;
	}
}