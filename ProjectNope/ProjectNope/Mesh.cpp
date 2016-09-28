#include "Mesh.h"
#include "GUIObject.h"
#include "Texture.h"
#include "RenderSetup.h"

Vertex::Vertex(const Vec3& pos) : p(pos){}

Face::Face(int va, int vb, int vc) : a(va), b(vb), c(vc), ta(-1), tb(-1), tc(-1){}

void Face::flip(void)
{
	n = -n;
	int tmp = a;
	a = c;
	c = tmp;
}

VBO::VBO(void) : v(0), t(0), n(0){}

Mesh::Mesh(void) : vbo_latest(false)
{
}

Mesh::Mesh(instream& is) : vbo_latest(false)
{
	bool first_mat = true;
	sets.push_back(std::vector<Face>());
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
			Face f(a, b, c);
			sets.back().push_back(f);
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
			if (sets.back().back().ta<0) {
				sets.back().back().ta = i;
			} else {
				if (sets.back().back().tb<0) {
					sets.back().back().tb = i;
				} else {
					if (sets.back().back().tc<0) {
						sets.back().back().tc = i;
					}
				}
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
				sets.push_back(std::vector<Face>());
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

	for (auto set = sets.begin();set!=sets.end();++set)
	{
		for (auto face = set->begin();face!=set->end();++face)
		{
			face->n = (vert[face->b].p-vert[face->a].p).Cross(vert[face->c].p-vert[face->a].p);
			face->n.Normalize();
			if (face->ta<0)
				face->ta=0;
			if (face->tb<0)
				face->tb=0;
			if (face->tc<0)
				face->tc=0;
		}
	}
	if (uv.size()==0)
		uv.push_back(Vec2(0.0f, 0.0f));
}

Mesh::Mesh(const Mesh& mesh)
{
	*this = mesh;
	vbos.clear();
	vbo_latest = false;
}

Mesh::~Mesh(void)
{
	for (auto i=vbos.begin();i!=vbos.end();++i)
	{
		if (i->v!=0)
			glDeleteBuffers(1, &i->v);
		if (i->t!=0)
			glDeleteBuffers(1, &i->t);
		if (i->n!=0)
			glDeleteBuffers(1, &i->n);
	}
}

#include "Profiler.h"

void Mesh::render(RenderSetup& rs)
{
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	for (int i=0;i<sets.size();++i)
	{
		LARGE_INTEGER freq, start, end;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&start);

		if (i<tex.size()) {
			if (tex[i]!=0) {
				tex[i]->sRGB = true;
				glBindTexture(GL_TEXTURE_2D, tex[i]->getGLTexID());
			}
		}

		if (!vbo_latest)
			buildVBO();

		rs.applyMods();

		GLint pos = rs.current_program->GetAttributeLocation("pos");
		GLint normal = rs.current_program->GetAttributeLocation("normal");
		GLint texcoord = rs.current_program->GetAttributeLocation("texCoord");

		QueryPerformanceCounter(&end);
		double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
		Profiler::add("mesh-partial", durationInSeconds);

		glEnableVertexAttribArray(pos);
		glEnableVertexAttribArray(normal);
		glEnableVertexAttribArray(texcoord);

		glBindBuffer(GL_ARRAY_BUFFER, vbos[i].v);
		glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, vbos[i].t);
		glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, vbos[i].n);
		glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glDrawArrays(GL_TRIANGLES, 0, sets[i].size()*3);

		glDisableVertexAttribArray(texcoord);
		glDisableVertexAttribArray(normal);
		glDisableVertexAttribArray(pos);

		/*glBegin(GL_TRIANGLES);

		for (std::vector<Face>::const_iterator it = sets[i].cbegin();it!=sets[i].cend();++it)
		{
			{
				const Vec2& tc = uv[it->ta];
				const Vec3& n = vert[it->a].n;
				const Vec3& p = vert[it->a].p;
				glTexCoord2f(tc.x, tc.y);
				glNormal3f(n.x, n.y, n.z);
				glVertex3f(p.x, p.y, p.z);
			}
			{
				const Vec2& tc = uv[it->tb];
				const Vec3& n = vert[it->b].n;
				const Vec3& p = vert[it->b].p;
				glTexCoord2f(tc.x, tc.y);
				glNormal3f(n.x, n.y, n.z);
				glVertex3f(p.x, p.y, p.z);
			}
			{
				const Vec2& tc = uv[it->tc];
				const Vec3& n = vert[it->c].n;
				const Vec3& p = vert[it->c].p;
				glTexCoord2f(tc.x, tc.y);
				glNormal3f(n.x, n.y, n.z);
				glVertex3f(p.x, p.y, p.z);
			}
		}

		glEnd();*/
	}

	QueryPerformanceCounter(&end);
	double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	Profiler::add("mesh-render", durationInSeconds);
}

void Mesh::render(RenderSetup& rs, const Pose& pose)
{
	Mesh * mesh = new Mesh(*this);
	getPose(pose, mesh);
	mesh->render(rs);
	delete mesh;
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

void Mesh::slotTexture(int slot, const std::shared_ptr<Texture>& texture)
{
	if (slot>=tex.size())
		tex.resize(slot+1);
	tex[slot]=texture;
}

void Mesh::addVBO(size_t set, Vec3 * v_data, Vec3 * n_data, Vec2 * t_data, Matrix4 mtrx)
{
	std::vector<Face>& setref = sets[set];

	for (auto p = setref.begin(); p != setref.end(); ++p)
	{
		v_data[(p - setref.begin()) * 3] = vert[p->a].p * mtrx;
		v_data[(p - setref.begin()) * 3 + 1] = vert[p->b].p * mtrx;
		v_data[(p - setref.begin()) * 3 + 2] = vert[p->c].p * mtrx;
	}

	for (auto p = setref.begin(); p != setref.end(); ++p)
	{
		n_data[(p - setref.begin()) * 3] = vert[p->a].n * Matrix3(mtrx);
		n_data[(p - setref.begin()) * 3 + 1] = vert[p->b].n * Matrix3(mtrx);
		n_data[(p - setref.begin()) * 3 + 2] = vert[p->c].n * Matrix3(mtrx);
	}

	for (auto p = setref.begin(); p != setref.end(); ++p)
	{
		t_data[(p - setref.begin()) * 3] = uv[p->ta];
		t_data[(p - setref.begin()) * 3 + 1] = uv[p->tb];
		t_data[(p - setref.begin()) * 3 + 2] = uv[p->tc];
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
		std::vector<Face>& setref = sets[i-vbos.begin()];

		//generate data
		Vec3 * v_data = new Vec3[setref.size()*3];
		for (auto p=setref.begin();p!=setref.end();++p)
		{
			v_data[(p-setref.begin())*3] = vert[p->a].p;
			v_data[(p-setref.begin())*3+1] = vert[p->b].p;
			v_data[(p-setref.begin())*3+2] = vert[p->c].p;
		}

		Vec3 * n_data = new Vec3[setref.size()*3];
		for (auto p=setref.begin();p!=setref.end();++p)
		{
			n_data[(p-setref.begin())*3] = vert[p->a].n;
			n_data[(p-setref.begin())*3+1] = vert[p->b].n;
			n_data[(p-setref.begin())*3+2] = vert[p->c].n;
		}

		Vec2 * t_data = new Vec2[setref.size()*3];
		for (auto p=setref.begin();p!=setref.end();++p)
		{
			t_data[(p-setref.begin())*3] = uv[p->ta];
			t_data[(p-setref.begin())*3+1] = uv[p->tb];
			t_data[(p-setref.begin())*3+2] = uv[p->tc];
		}

		//maybe make the vbos interleaved

		//generate vbos and pass data
		if (i->v == 0)
			glGenBuffers(1, &i->v);
		glBindBuffer(GL_ARRAY_BUFFER, i->v);
		glBufferData(GL_ARRAY_BUFFER, setref.size()*3*sizeof(Vec3), v_data, GL_DYNAMIC_DRAW);
		if (i->t == 0)
			glGenBuffers(1, &i->t);
		glBindBuffer(GL_ARRAY_BUFFER, i->t);
		glBufferData(GL_ARRAY_BUFFER, setref.size()*3*sizeof(Vec2), t_data, GL_STATIC_DRAW);
		if (i->n == 0)
			glGenBuffers(1, &i->n);
		glBindBuffer(GL_ARRAY_BUFFER, i->n);
		glBufferData(GL_ARRAY_BUFFER, setref.size()*3*sizeof(Vec3), n_data, GL_DYNAMIC_DRAW);

		delete [] v_data;
		delete [] n_data;
		delete [] t_data;

		vbo_latest = true;
	}

	QueryPerformanceCounter(&end);
	double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	Profiler::add("vbo-build", durationInSeconds);
}