#include "GraphicsComponent.h"

#include <algorithm>

#include "NewEntity.h"

#include "PositionComponent.h"
#include "PoseComponent.h"

#include "RenderSetup.h"

AutoSerialFactory<GraphicsComponent, Component> GraphicsComponent::_factory("GraphicsComponent");

thread_local std::vector<GraphicsComponent*> GraphicsComponent::all;

GraphicsComponent::GraphicsComponent(bool dynamic, uint32_t tag) : Component(_factory.id), p(nullptr), pose(nullptr)
{
	this->dynamic = dynamic;
	this->tag = tag;
}

GraphicsComponent::GraphicsComponent(instream& is, bool full) : Component(_factory.id), p(nullptr), pose(nullptr)
{
	all.push_back(this);
	is >> dynamic >> tag;
	uint32_t size;
	is >> size;
	decs.items.resize(size);
	for (size_t i = 0; i < size; ++i)
	{
		bool exists;
		is >> exists;
		if (exists)
		{
			decs.items[i] = std::make_shared<Decorator>();
			decs.items[i]->readLog(is);
		}
	}
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
	decs.setSyncState(nullptr);
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

void GraphicsComponent::pre_frame(float dTime)
{
	angle += Vec3(1.0f, M_PI / 10.0f, M_E) * dTime;

	if (p == nullptr)
		p = entity->getComponent<PositionComponent>();

	for (auto i = decs.items.begin(); i != decs.items.end(); ++i)
	{
		if (*i != nullptr)
		{
			(*i)->attach();
		}
	}
}

void GraphicsComponent::interpolate(Component * pComponent, float fWeight)
{
	GraphicsComponent * pGraphicsComponent = dynamic_cast<GraphicsComponent*>(pComponent);
	if (pGraphicsComponent != nullptr)
	{
		decs = pGraphicsComponent->decs;
	}
}

void GraphicsComponent::write_to(outstream& os, ClientData& client) const
{
	os << dynamic << tag;
	os << (uint32_t)decs.items.size();
	for (auto item : decs.items)
	{
		if (item)
		{
			os << true;
			item->writeLog(os);
		}
		else
		{
			os << false;
		}
	}
}

void GraphicsComponent::write_to(outstream& os) const
{
}

void GraphicsComponent::render(RenderSetup& rs)
{
	rs.pushTransform();

	GlobalPosition offset = -rs.origin;
	if (p == nullptr)
		p = entity->getComponent<PositionComponent>();
	if (p != nullptr)
		offset += p->p;
	rs.addTransform(Matrix4::Translation(offset));

	auto pose = entity->getComponent<PoseComponent>();
	if (pose)
	{
		if (pose->pose.bones.size())
		{
			for (auto i = decs.items.begin(); i != decs.items.end(); ++i)
			{
				if (*i != nullptr)
				{
					(*i)->render(rs, pose->pose);
				}
			}
		}
	}
	else
	{
		for (auto i = decs.items.begin(); i != decs.items.end(); ++i)
		{
			if (*i != nullptr)
			{
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
	/*if (!dynamic)
	{
		for (auto j = decs.items.begin(); j != decs.items.end(); ++j)
		{
			if ((*j)->skin != 0)
			{
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
	}*/
}

std::vector<GraphicsComponent*> standard;

class InstancedGraphics
{
public:
	InstancedGraphics(const std::string& mesh, const MaterialList& mats, const std::string& anim)
	{
		instance_mesh_name = mesh;
		instance_materials = mats;
		instance_animation_name = anim;
		instance_count = 0;
	}

	void clear(void)
	{
		instance_transforms.clear();
		instance_frames.clear();
		instance_count = 0;
	}

	void render(RenderSetup& rs)
	{
		instance_mesh = Resource::get<Mesh>(instance_mesh_name);
		instance_animation = Resource::get<SkeletalAnimation>(instance_animation_name);

		rs.applyMods();
		auto instance_shader = ShaderProgram::Get(Shader::get("data/gfill_skinned_instanced_vert.txt", SHADER_VERTEX), rs.current_program->geom, rs.current_program->frag);
		if (tag == 1 && rs.pass == 5)
		{
			instance_shader = ShaderProgram::Get(Shader::get("data/gfill_skinned_instanced_vert.txt", SHADER_VERTEX), rs.current_program->geom, Shader::get("data/gfill_ao_simple_frag.txt", SHADER_FRAGMENT));
		}

		if (instance_shader->IsReady() && instance_animation && instance_mesh)
		{
			auto instance_animation_texture = instance_animation->getCompiledTexture();

			if (instance_animation_texture)
			{
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, instance_animation_texture->texid);

				for (size_t i = 0; i < instance_count; i += 64)
				{
					size_t nInstancesToRender = min(64, instance_count - i);

					ShaderMod mod(instance_shader, [&rs, instance_animation_texture, i, nInstancesToRender, this](const std::shared_ptr<ShaderProgram>& prog) {
						prog->UniformMatrix4fv("instance_transform", nInstancesToRender, (instance_transforms.begin() + i)._Ptr);
						prog->Uniform1fv("instance_frame", nInstancesToRender, (instance_frames.begin() + i)._Ptr);
						prog->Uniform("animation_data", 2);
						prog->Uniform("bone_count", (float)instance_animation_texture->w);
						prog->Uniform("frame_count", (float)instance_animation_texture->h);
						prog->Uniform("ao", 1);
					});

					rs.pushMod(mod);

					instance_mesh->render_instanced(rs, instance_materials, nInstancesToRender);

					rs.popMod();
				}
			}
		}
	}

	void add(const Matrix4& transform, float frame)
	{
		instance_transforms.push_back(transform);
		instance_frames.push_back(frame);
		++instance_count;
	}

	std::string instance_mesh_name;
	std::string instance_animation_name;

	std::shared_ptr<Mesh> instance_mesh;
	MaterialList instance_materials;
	std::shared_ptr<SkeletalAnimation> instance_animation;

	std::vector<Matrix4> instance_transforms;
	std::vector<float> instance_frames;
	size_t instance_count;

	uint32_t tag;
};

std::map<std::pair<int, std::tuple<std::string, MaterialList, std::string>>, std::shared_ptr<InstancedGraphics>> instanced;

void GraphicsComponent::prep(RenderSetup& rs)
{
	TimeslotA(prep);

	for each (auto type in instanced)
	{
		type.second->clear();
	}

	standard.clear();

	for each (auto g in all)
	{
		if (g->decs.items.size())
		{
			auto pose = g->entity->getComponent<PoseComponent>();
			if (false)//(pose)
			{
				for (auto dec : g->decs.items)
				{
					if (dec->bone_id == -1)
					{
						auto key = std::make_pair(dec->priority, std::make_tuple(dec->mesh_fname, dec->materials, pose->anim));
						auto it = instanced.find(key);
						std::shared_ptr<InstancedGraphics> ig;
						if (it != instanced.end())
						{
							ig = it->second;
						}
						else
						{
							ig.reset(new InstancedGraphics(dec->mesh_fname, dec->materials, pose->anim));
							ig->tag = g->tag;
							instanced.insert(std::make_pair(key, ig));
						}
						if (g->p)
							ig->add(dec->local * Matrix4::Translation(g->p->p - rs.origin), pose->frame);
					}
					else
					{
					}
				}
			}
			else
			{
				standard.push_back(g);
			}
		}
	}

	/*for (size_t i = 0; i < 20000; i++)
	{
		instance_transforms.push_back(Matrix4::Translation(Vec3(50.0f, 50.0f, 50.0f) - rs.origin));
		instance_frames.push_back(50.0f);
		++instance_count;
	}*/

	//standard_dynamic.clear();
	//custom_dynamic.clear();

	//std::map<render_priority, std::vector<std::pair<Mesh*, std::pair<size_t, Matrix4>>>> _statics;

	//std::set<render_priority> ready = complete;

	//for (auto i=all.begin();i!=all.end();++i)
	//{
	//	if ((*i)->mod.size() == 0)
	//	{
	//		if ((*i)->dynamic)
	//		{
	//			for (auto j = (*i)->decs.items.begin(); j != (*i)->decs.items.end(); ++j)
	//			{
	//				if ((*j)->skin != 0)
	//				{
	//					if (!(*j)->skin->vbo_latest)
	//						(*j)->skin->buildVBO();
	//					for (size_t k = 0; k != (*j)->skin->vbos.size(); ++k)
	//					{
	//						if (k < (*j)->skin->tex.size())
	//						{
	//							if ((*j)->skin->tex[k] != 0)
	//							{
	//								render_priority prio((*j)->priority, (*j)->_tex[k]);
	//								auto& ref_pair = standard_dynamic[prio];
	//								ref_pair.first = (*j)->skin->tex[k]->getGLTexID();
	//								auto& ref = ref_pair.second;
	//								ref.push_back(PreparedVBO());
	//								if ((*i)->p != 0)
	//									ref.back().p = *(*i)->p;
	//								ref.back().mtrx = (*j)->final;
	//								ref.back().v = (*j)->skin->vbos[k].v;
	//								ref.back().t = (*j)->skin->vbos[k].t;
	//								ref.back().n = (*j)->skin->vbos[k].n;
	//								ref.back().size = (*j)->skin->sets[k].size() * 3;
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//		else
	//		{
	//			for (auto j = (*i)->decs.items.begin(); j != (*i)->decs.items.end(); ++j)
	//			{
	//				if ((*j)->skin != 0)
	//				{
	//					for (int i = 0; i < (*j)->_tex.size(); ++i) {
	//						std::shared_ptr<Texture> tex = Resource::get<Texture>((*j)->_tex[i]);
	//						if (tex != 0)
	//							tex->sRGB = true;
	//						(*j)->skin->slotTexture(i, tex);
	//					}
	//					for (size_t k = 0; k != (*j)->skin->sets.size(); ++k)
	//					{
	//						if (k < (*j)->skin->tex.size())
	//						{
	//							if ((*j)->skin->tex[k] != 0)
	//							{
	//								render_priority prio((*j)->priority, (*j)->_tex[k]);
	//								if (ready.find(prio) == ready.end())
	//								{
	//									_statics[prio].push_back(std::make_pair((*j)->skin.get(), std::make_pair(k, (*j)->final*Matrix4::Translation(*(*i)->p))));
	//									auto& pair_ref = standard_static[prio];
	//									pair_ref.first = (*j)->skin->tex[k]->getGLTexID();
	//									pair_ref.second.size += (*j)->skin->sets[k].size();
	//									complete.insert(prio);
	//								}
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//	}
	//	else
	//	{
	//		custom_dynamic.push_back(*i);
	//	}
	//}

	//for (auto i = _statics.begin(); i != _statics.end(); ++i)
	//{
	//	PreparedVBO& pvbo = standard_static[i->first].second;

	//	pvbo.size *= 3;

	//	Vec3 * v_data = new Vec3[pvbo.size];
	//	Vec3 * n_data = new Vec3[pvbo.size];
	//	Vec2 * t_data = new Vec2[pvbo.size];

	//	Vec3 * v_data_offset = v_data;
	//	Vec3 * n_data_offset = n_data;
	//	Vec2 * t_data_offset = t_data;

	//	//std::cout << pvbo.size << std::endl;
	//	//std::cout << pvbo.size * sizeof(Vec3) << std::endl;

	//	for (auto j = i->second.begin(); j != i->second.end(); ++j)
	//	{
	//		size_t offset = j->first->sets[j->second.first].size() * 3;
	//		j->first->addVBO(j->second.first, v_data_offset, n_data_offset, t_data_offset, j->second.second);
	//		v_data_offset += offset;
	//		n_data_offset += offset;
	//		t_data_offset += offset;
	//	}

	//	//std::cout << (int)(v_data_offset - v_data) << std::endl;

	//	if (pvbo.v == 0)
	//		glGenBuffers(1, &pvbo.v);
	//	glBindBuffer(GL_ARRAY_BUFFER, pvbo.v);
	//	glBufferData(GL_ARRAY_BUFFER, pvbo.size * sizeof(Vec3), v_data, GL_STATIC_DRAW);
	//	if (pvbo.t == 0)
	//		glGenBuffers(1, &pvbo.t);
	//	glBindBuffer(GL_ARRAY_BUFFER, pvbo.t);
	//	glBufferData(GL_ARRAY_BUFFER, pvbo.size * sizeof(Vec2), t_data, GL_STATIC_DRAW);
	//	if (pvbo.n == 0)
	//		glGenBuffers(1, &pvbo.n);
	//	glBindBuffer(GL_ARRAY_BUFFER, pvbo.n);
	//	glBufferData(GL_ARRAY_BUFFER, pvbo.size * sizeof(Vec3), n_data, GL_STATIC_DRAW);

	//	delete[] v_data;
	//	delete[] n_data;
	//	delete[] t_data;
	//}
}

std::shared_ptr<ShaderProgram> ao_shader;
std::shared_ptr<ShaderProgram> swing_shader;
std::shared_ptr<ShaderProgram> grass_shader;
std::shared_ptr<ShaderProgram> water_shader;

void GraphicsComponent::render_all(RenderSetup& rs)
{
	if (!ao_shader)
		ao_shader = std::make_shared<ShaderProgram>("data/gfill_ao_vert.txt", "data/gfill_ao_frag.txt");
	if (!swing_shader)
		swing_shader = std::make_shared<ShaderProgram>("data/gfill_vert.txt", "data/swing_frag.txt");
	if (!grass_shader)
		grass_shader = std::make_shared<ShaderProgram>("data/grass_vert.txt", "data/grass_frag.txt");
	if (!water_shader)
		water_shader = std::make_shared<ShaderProgram>("data/gfill_vert.txt", "data/water_frag.txt");

	for each (auto g in standard)
	{
		bool shader = false;
		if (g->tag == 1 && rs.pass != 3 && rs.pass != 4)
		{
			rs.pushMod(ShaderMod(ao_shader, [=](const std::shared_ptr<ShaderProgram>& prog)
			{
				prog->Uniform("ao", 1);
			}));
			shader = true;
		}
		if (g->tag == 2)
		{
			if (rs.pass == 3 || rs.pass == 4)
				continue;
			rs.pushMod(ShaderMod(swing_shader, [=](const std::shared_ptr<ShaderProgram>& prog)
			{
				prog->Uniform("angle", g->angle);
				prog->Uniform("limit", -1.0f);// fmaxf(-1.0, fminf(1.0f, sinf(angle.x / 20.0f) * 5.0f)));
			}));
			shader = true;
		}
		if (g->tag == 3)
		{
			if (rs.pass == 3 || rs.pass == 4)
				continue;
			rs.pushMod(ShaderMod(grass_shader, [=](const std::shared_ptr<ShaderProgram>& prog)
			{
				prog->Uniform("angle", g->angle);
			}));
			shader = true;
		}
		if (g->tag == 4)
		{
			if (rs.pass == 3 || rs.pass == 4)
				continue;
			rs.pushMod(ShaderMod(water_shader, [=](const std::shared_ptr<ShaderProgram>& prog)
			{
				prog->Uniform("normal", 1);
				prog->Uniform("angle", g->angle);
			}));
			shader = true;
		}

		g->render(rs);

		if (shader)
		{
			rs.popMod();
		}
	}

	for each (auto type in instanced)
	{
		type.second->render(rs);
	}

	/*if (rs.applyMods())
	{

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

				glDrawArrays(GL_TRIANGLES, 0, vbos[j].vbo.nIndices);
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

	}*/
}