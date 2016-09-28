#include "GraphicsComponent.h"

#include "NewEntity.h"

#include "PositionComponent.h"

#include "RenderSetup.h"

const AutoSerialFactory<GraphicsComponent> GraphicsComponent::_factory("GraphicsComponent");

std::vector<GraphicsComponent*> GraphicsComponent::all;

std::map<render_priority, std::pair<unsigned int, std::vector<PreparedVBO>>> GraphicsComponent::standard_dynamic;
std::vector<GraphicsComponent*> GraphicsComponent::custom_dynamic;
std::map<render_priority, std::pair<unsigned int, PreparedVBO>> GraphicsComponent::standard_static;
std::set<render_priority> GraphicsComponent::complete;

GraphicsComponent::GraphicsComponent(bool dynamic) : Serializable(_factory.id), p(0)
{
	all.push_back(this);
	this->dynamic = dynamic;
}

GraphicsComponent::GraphicsComponent(instream& is, bool full) : Serializable(_factory.id), p(0)
{
	all.push_back(this);
	is >> dynamic;
}

GraphicsComponent::~GraphicsComponent(void)
{
	all.erase(std::remove(all.begin(), all.end(), this), all.end());
	invalidate();
}

void GraphicsComponent::connect(NewEntity * pEntity, bool authority)
{
	if (authority)
		decs.setSyncState(&pEntity->ss);
}

void GraphicsComponent::disconnect(void)
{
	decs.setSyncState(0);
}

void GraphicsComponent::writeLog(outstream& os, ClientData& client)
{
	if (decs.conf.size())
		decs.writeLog(os);
}

void GraphicsComponent::readLog(instream& is)
{
	decs.readLog(is);
}

void GraphicsComponent::frame(float dTime)
{
	if (p==0) {
		PositionComponent * pc = entity->getComponent<PositionComponent>();
		if (pc!=0)
			p = &pc->p;
	}

	if (pose.bones.size()) {
		for (auto i=decs.items.begin();i!=decs.items.end();++i) {
			if (*i!=0) {
				(*i)->attach(pose);
			}
		}
	} else {
		for (auto i=decs.items.begin();i!=decs.items.end();++i) {
			if (*i!=0) {
				(*i)->attach();
			}
		}
	}
}

void GraphicsComponent::interpolate(Component * pComponent, float fWeight)
{
	GraphicsComponent * pGraphicsComponent = dynamic_cast<GraphicsComponent*>(pComponent);
	if (pGraphicsComponent!=0) {
		decs = pGraphicsComponent->decs;
	}
}

void GraphicsComponent::write_to(outstream& os, ClientData& client) const
{
	os << dynamic;
}

void GraphicsComponent::write_to(outstream& os) const
{
}

void GraphicsComponent::render(RenderSetup& rs)
{
	rs.pushTransform();

	GlobalPosition offset = -rs.origin;
	if (p==0) {
		PositionComponent * pc = entity->getComponent<PositionComponent>();
		if (pc!=0)
			p = &pc->p;
	}
	if (p!=0)
		offset += *p;
	Vec3 pos = offset;
	rs.addTransform(Matrix4::Translation(pos));

	if (pose.bones.size()) {
		for (auto i=decs.items.begin();i!=decs.items.end();++i) {
			if (*i!=0) {
				(*i)->render(rs);
			}
		}
	} else {
		for (auto i=decs.items.begin();i!=decs.items.end();++i) {
			if (*i!=0) {
				(*i)->render(rs);
			}
		}
	}

	rs.popTransform();
}

#include "Profiler.h"

#include "Mesh.h"
#include "Texture.h"

void GraphicsComponent::invalidate(void)
{
	if (!dynamic)
	{
		for (auto j = decs.items.begin(); j != decs.items.end(); ++j)
		{
			if ((*j)->skin != 0)
			{
				for (int i = 0; i < (*j)->_tex.size(); ++i) {
					std::shared_ptr<Texture> tex = Resource::get<Texture>((*j)->_tex[i]);
					if (tex != 0)
						tex->sRGB = true;
					(*j)->skin->slotTexture(i, tex);
				}
				for (size_t k = 0; k != (*j)->skin->sets.size(); ++k)
				{
					if (k < (*j)->skin->tex.size())
					{
						if ((*j)->skin->tex[k] != 0)
						{
							render_priority prio((*j)->priority, (*j)->_tex[k]);
							complete.erase(prio);

							if (standard_static.find(prio) != standard_static.end())
							{
								PreparedVBO& pvbo = standard_static[prio].second;
								glDeleteBuffers(1, &pvbo.n);
								glDeleteBuffers(1, &pvbo.t);
								glDeleteBuffers(1, &pvbo.v);
								standard_static.erase(prio);
							}
						}
					}
				}
			}
		}
	}
}

void GraphicsComponent::prep(void)
{
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	standard_dynamic.clear();
	custom_dynamic.clear();

	std::map<render_priority, std::vector<std::pair<Mesh*, std::pair<size_t, Matrix4>>>> _statics;

	std::set<render_priority> ready = complete;

	for (auto i=all.begin();i!=all.end();++i)
	{
		if ((*i)->mod.size() == 0)
		{
			if ((*i)->dynamic)
			{
				for (auto j = (*i)->decs.items.begin(); j != (*i)->decs.items.end(); ++j)
				{
					if ((*j)->skin != 0)
					{
						for (int i = 0; i < (*j)->_tex.size(); ++i) {
							std::shared_ptr<Texture> tex = Resource::get<Texture>((*j)->_tex[i]);
							if (tex != 0)
								tex->sRGB = true;
							(*j)->skin->slotTexture(i, tex);
						}
						if (!(*j)->skin->vbo_latest)
							(*j)->skin->buildVBO();
						for (size_t k = 0; k != (*j)->skin->vbos.size(); ++k)
						{
							if (k < (*j)->skin->tex.size())
							{
								if ((*j)->skin->tex[k] != 0)
								{
									render_priority prio((*j)->priority, (*j)->_tex[k]);
									auto& ref_pair = standard_dynamic[prio];
									ref_pair.first = (*j)->skin->tex[k]->getGLTexID();
									auto& ref = ref_pair.second;
									ref.push_back(PreparedVBO());
									if ((*i)->p != 0)
										ref.back().p = *(*i)->p;
									ref.back().mtrx = (*j)->final;
									ref.back().v = (*j)->skin->vbos[k].v;
									ref.back().t = (*j)->skin->vbos[k].t;
									ref.back().n = (*j)->skin->vbos[k].n;
									ref.back().size = (*j)->skin->sets[k].size() * 3;
								}
							}
						}
					}
				}
			}
			else
			{
				for (auto j = (*i)->decs.items.begin(); j != (*i)->decs.items.end(); ++j)
				{
					if ((*j)->skin != 0)
					{
						for (int i = 0; i < (*j)->_tex.size(); ++i) {
							std::shared_ptr<Texture> tex = Resource::get<Texture>((*j)->_tex[i]);
							if (tex != 0)
								tex->sRGB = true;
							(*j)->skin->slotTexture(i, tex);
						}
						for (size_t k = 0; k != (*j)->skin->sets.size(); ++k)
						{
							if (k < (*j)->skin->tex.size())
							{
								if ((*j)->skin->tex[k] != 0)
								{
									render_priority prio((*j)->priority, (*j)->_tex[k]);
									if (ready.find(prio) == ready.end())
									{
										_statics[prio].push_back(std::make_pair((*j)->skin.get(), std::make_pair(k, (*j)->final*Matrix4::Translation(*(*i)->p))));
										auto& pair_ref = standard_static[prio];
										pair_ref.first = (*j)->skin->tex[k]->getGLTexID();
										pair_ref.second.size += (*j)->skin->sets[k].size();
										complete.insert(prio);
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			custom_dynamic.push_back(*i);
		}
	}

	for (auto i = _statics.begin(); i != _statics.end(); ++i)
	{
		PreparedVBO& pvbo = standard_static[i->first].second;

		pvbo.size *= 3;

		Vec3 * v_data = new Vec3[pvbo.size];
		Vec3 * n_data = new Vec3[pvbo.size];
		Vec2 * t_data = new Vec2[pvbo.size];

		Vec3 * v_data_offset = v_data;
		Vec3 * n_data_offset = n_data;
		Vec2 * t_data_offset = t_data;

		//std::cout << pvbo.size << std::endl;
		//std::cout << pvbo.size * sizeof(Vec3) << std::endl;

		for (auto j = i->second.begin(); j != i->second.end(); ++j)
		{
			size_t offset = j->first->sets[j->second.first].size() * 3;
			j->first->addVBO(j->second.first, v_data_offset, n_data_offset, t_data_offset, j->second.second);
			v_data_offset += offset;
			n_data_offset += offset;
			t_data_offset += offset;
		}

		//std::cout << (int)(v_data_offset - v_data) << std::endl;

		if (pvbo.v == 0)
			glGenBuffers(1, &pvbo.v);
		glBindBuffer(GL_ARRAY_BUFFER, pvbo.v);
		glBufferData(GL_ARRAY_BUFFER, pvbo.size * sizeof(Vec3), v_data, GL_STATIC_DRAW);
		if (pvbo.t == 0)
			glGenBuffers(1, &pvbo.t);
		glBindBuffer(GL_ARRAY_BUFFER, pvbo.t);
		glBufferData(GL_ARRAY_BUFFER, pvbo.size * sizeof(Vec2), t_data, GL_STATIC_DRAW);
		if (pvbo.n == 0)
			glGenBuffers(1, &pvbo.n);
		glBindBuffer(GL_ARRAY_BUFFER, pvbo.n);
		glBufferData(GL_ARRAY_BUFFER, pvbo.size * sizeof(Vec3), n_data, GL_STATIC_DRAW);

		delete[] v_data;
		delete[] n_data;
		delete[] t_data;
	}

	QueryPerformanceCounter(&end);
	double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	Profiler::add("prep", durationInSeconds);
}

void GraphicsComponent::render_all(RenderSetup& rs)
{
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	rs.applyMods();

	GLint pos = rs.current_program->GetAttributeLocation("pos");
	GLint normal = rs.current_program->GetAttributeLocation("normal");
	GLint texcoord = rs.current_program->GetAttributeLocation("texCoord");

	glEnableVertexAttribArray(pos);
	glEnableVertexAttribArray(normal);
	glEnableVertexAttribArray(texcoord);

	for (auto i = standard_static.begin(); i != standard_static.end(); ++i)
	{
		glBindTexture(GL_TEXTURE_2D, i->second.first);
		PreparedVBO& vbo = i->second.second;

		rs.current_program->UniformMatrix4fv("transform", (Matrix4::Translation(-rs.origin)*rs.view).data);
		rs.current_program->UniformMatrix3fv("normal_transform", Matrix3().data);

		glBindBuffer(GL_ARRAY_BUFFER, vbo.v);
		glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo.t);
		glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
		glBindBuffer(GL_ARRAY_BUFFER, vbo.n);
		glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

		glDrawArrays(GL_TRIANGLES, 0, vbo.size);
	}

	for (auto i = standard_dynamic.begin(); i != standard_dynamic.end(); ++i)
	{
		glBindTexture(GL_TEXTURE_2D, i->second.first);
		std::vector<PreparedVBO>& vbos = i->second.second;
		for (size_t j = 0; j != vbos.size(); ++j)
		{
			rs.current_program->UniformMatrix4fv("transform", (vbos[j].mtrx*Matrix4::Translation(vbos[j].p - rs.origin)*rs.view).data);
			rs.current_program->UniformMatrix3fv("normal_transform", Matrix3(vbos[j].mtrx).data);

			glBindBuffer(GL_ARRAY_BUFFER, vbos[j].v);
			glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			glBindBuffer(GL_ARRAY_BUFFER, vbos[j].t);
			glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			glBindBuffer(GL_ARRAY_BUFFER, vbos[j].n);
			glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

			glDrawArrays(GL_TRIANGLES, 0, vbos[j].size);
		}
	}

	glDisableVertexAttribArray(texcoord);
	glDisableVertexAttribArray(normal);
	glDisableVertexAttribArray(pos);

	for (auto i = custom_dynamic.begin(); i != custom_dynamic.end(); ++i)
	{
		if ((*i)->mod.find(rs.pass) != (*i)->mod.end())
		{
			rs.pushMod((*i)->mod[rs.pass]);
			(*i)->render(rs);
			rs.popMod();
		}
		else
		{
			(*i)->render(rs);
		}
	}

	QueryPerformanceCounter(&end);
	double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	Profiler::add("draw calls", durationInSeconds);
}