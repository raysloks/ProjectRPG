#include "Server.h"
#include "Client.h"
#include "GUIObject.h"
#include <iostream>
#include "streams.h"
#include <fstream>

#include "StaticProp.h"

#include "PositionComponent.h"

#include "World.h"
#include "Chunk.h"

#include "ClientData.h"

World::World(void)
{
	authority = true;
	use_chunks = true;
}

World::~World(void)
{
	clean();
	for (int i=0;i<units.size();++i) {
		NewEntity * unit = units[i];
		if (unit!=0)
			delete unit;
	}
	units.clear();
	for (auto i=chunks.begin();i!=chunks.end();++i) {
		Chunk * chunk = i->second;
		if (chunk!=0)
			delete chunk;
	}
	chunks.clear();
}

void World::frame(float dTime)
{
	for (int i=0;i<units.size();++i) {
		NewEntity * ent = units[i];
		if (ent!=0)
			ent->frame(dTime);
	}
}

void World::tick(float dTime)
{
	std::vector<int> pos;
	bool chunk_loaded;
	for (int i=0;i<units.size();++i) {
		NewEntity * ent = units[i];
		if (ent!=0) {
			if (use_chunks) {
				chunk_loaded = true;
				PositionComponent * p = ent->getComponent<PositionComponent>();
				if (p!=0)
				{
					chunk_loaded = false;
					pos.clear();
					pos.push_back((p->p.x)/(chunk_size));
					pos.push_back((p->p.y)/(chunk_size));
					pos.push_back((p->p.z)/(chunk_size));
					if (!authority)
						chunk_loaded = true;
					if (chunks[pos]!=0)
						chunk_loaded = !chunks[pos]->loading;
				}
				ent->tick(chunk_loaded ? dTime : 0.0f);
			} else {
				ent->tick(dTime);
			}
		}
	}
}

void World::clean(void)
{
	for (auto i=removed.begin();i!=removed.end();++i)
		delete *i;
	removed.clear();
}

void World::clear(void)
{
	clean();
	for (auto i=units.begin();i!=units.end();++i) {
		if (*i!=0)
			delete *i;
	}
	units.clear();
	uid.clear();
	while (alloc.size())
		alloc.pop();
	for (auto i=chunks.begin();i!=chunks.end();++i) {
		Chunk * chunk = i->second;
		if (chunk!=0)
			delete chunk;
	}
	chunks.clear();
	mem.reset();
}

void World::LoadChunk(const std::vector<int>& pos)
{
	if (chunks[pos]==0)
	{
		std::vector<std::string> tex;
		tex.push_back("data/assets/terrain/textures/grass.tga");
		tex.push_back("data/assets/terrain/textures/RockPlate.tga");
		tex.push_back("data/assets/terrain/textures/temple.tga");

		/*if (low==0)
		{
			Matrix3 mtrx;
			mtrx *= 600.0f;
			low = new StaticProp(GlobalPosition(), std::string("data/assets/terrain/") + level + "/low.gmdl", tex, mtrx);
			low->collision = false;
			low->camera_collision = false;
			low->range = 0.0f;
			world->AddEntity(low);
		}*/

		std::string chunk = std::to_string((_Longlong)pos[0]) + std::string("_") + std::to_string((_Longlong)pos[1]) + std::string("_") + std::to_string((_Longlong)pos[2]) + std::string(".dat");

		std::string fname = std::string("save/") + save + "/chunks/" + chunk;
		std::ifstream f(fname);
		if (f.good())
		{
			f.close();
			chunks[pos] = new Chunk(this, pos, fname);
		}
		else
		{
			std::string fname = std::string("data/level/") + level + "/" + chunk;
			std::ifstream f(fname);
			if (f.good())
			{
				f.close();
				chunks[pos] = new Chunk(this, pos, fname);
				return;
			}

			chunks[pos] = new Chunk(this, pos);

			/*PNEntity * terrain = new StaticProp(GlobalPosition(Vec3(), pos[0]*chunk_size, pos[1]*chunk_size, pos[2]*chunk_size), std::string("data/assets/terrain/") + level + "/high_" +
				std::to_string((long long)pos[0]) + "_" + std::to_string((long long)pos[1]) + "_" + std::to_string((long long)pos[2]) + ".gmdl", tex);
			chunks[pos]->add(AddEntity(terrain));*/
		}
	}
}

void World::LoadSurroundings(NewEntity * ent)
{
	if (ent!=0 && use_chunks)
	{
		PositionComponent * p = ent->getComponent<PositionComponent>();
		if (p!=0)
		{
			int dist = 1;
			std::vector<int> pos;
			for (int x = -dist;x<dist;++x)
			{
				for (int y = -dist;y<dist;++y)
				{
					for (int z = -dist;z<dist;++z)
					{
						pos.clear();
						pos.push_back(std::floorf((p->p.x+chunk_size_half)/(chunk_size))+x);
						pos.push_back(std::floorf((p->p.y+chunk_size_half)/(chunk_size))+y);
						pos.push_back(std::floorf((p->p.z+chunk_size_half)/(chunk_size))+z);
						LoadChunk(pos);
					}
				}
			}
		}
	}
}

NewEntity * World::NearestPlayer(const GlobalPosition& pos)
{
	NewEntity * player = 0;
	float distance = 0.0f;

	if (client!=0)
	{
		if (client->clientData!=0)
		{
			NewEntity * ent = GetEntity(client->clientData->unit_id);
			if (ent!=0)
			{
				PositionComponent * p = ent->getComponent<PositionComponent>();
				if (p!=0) {
					player = ent;
					distance = Vec3(p->p-pos).LenPwr();
				}
			}
		}
	}

	for (auto i=server->conns.begin();i!=server->conns.end();++i)
	{
		NewEntity * ent = GetEntity(i->second->data->unit_id);
		if (ent!=0)
		{
			PositionComponent * p = ent->getComponent<PositionComponent>();
			if (p!=0) {
				float l = Vec3(p->p-pos).LenPwr();
				if (player!=0)
				{
					if (l<distance)
					{
						player = ent;
						distance = l;
					}
				}
				else
				{
					player = ent;
					distance = l;
				}
			}
		}
	}

	return player;
}

void World::UnloadChunk(const std::vector<int>& pos)
{
	if (chunks[pos]!=0)
	{
		std::string folder = "save/" + save + "/chunks/";
		chunks[pos]->save(folder);
		chunks[pos]->unload();
	}
}

#include "Profiler.h"

#include "GraphicsComponent.h"

#include "RenderSetup.h"

void World::render(RenderSetup& rs)
{
	LARGE_INTEGER freq, start, end;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	if (rs.tmp_use_default_state)
	{
		glEnable(GL_DEPTH_TEST);

		glEnable(GL_DEPTH_CLAMP);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glEnable(GL_TEXTURE_2D);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.0f);
	}

	rs.origin = cam_pos;

	GraphicsComponent::render_all(rs);

	// TODO component rendering
	/*for (int i=0;i<units.size();++i) {
		if (units[i]!=0)
			units[i]->render(cam_pos);
	}*/

	QueryPerformanceCounter(&end);
	double durationInSeconds = static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
	Profiler::add("world-render-pass", durationInSeconds);
}

int World::AddEntity(NewEntity * unit)
{
	if (unit!=0 && authority) {
		int id = -1;
		while (id==-1)
		{
			if (alloc.size()) {
				int i = alloc.top();
				alloc.pop();
				if (units[i]==0) {
					id = i;
				}
			} else {
				break;
			}
		}
		if (id==-1)
		{
			for (int i=0;i<units.size();++i) {
				if (units[i]==0) {
					id = i;
					break;
				}
			}
		}
		if (id==-1)
		{
			id = units.size();
			unit->id = id;
			unit->world = this;
			units.push_back(unit);
			uid.push_back(0);
			if (server!=0)
				server->NotifyOfCreation(id);
		}
		else
		{
			unit->id = id;
			unit->world = this;
			units[id] = unit;
			if (server!=0)
				server->NotifyOfCreation(id);
		}
		return id;
	}
	return -1;
}

void World::SetEntity(int id, NewEntity * unit)
{
	try
	{
		if (id<0)
			return;
		if (unit!=0) {
			if (id>=units.size()) {
				units.resize(id+1);
				uid.resize(id+1);
			}
			unit->id = id;
			unit->world = this;
			if (uid[id]==INT_MAX)
				uid[id]=INT_MIN;
			else
				uid[id]++;
		}
		if (units.at(id)!=0) {
			if (server!=0 && authority)
				server->NotifyOfRemoval(id, uid[id]);
			/*if (units[id]->chunk!=0)
				units[id]->chunk->remove(units[id]->cid);*/
			removed.push_back(units[id]);
			alloc.push(id);
		}
		units[id] = unit;
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

NewEntity * World::GetEntity(int id)
{
	if (id>=units.size() || id<0)
		return 0;
	return units[id];
}

NewEntity * World::GetEntity(int id, int unid)
{
	if (id>=units.size() || id<0)
		return 0;
	if (units[id]!=0)
		if (uid[id]!=unid)
			return 0;
	return units[id];
}

GlobalPosition World::getMed(const std::set<int>& e)
{
	std::set<int> ents = e;
	GlobalPosition med;
	int n = 0;
	for (std::set<int>::iterator it=ents.begin();it!=ents.end();++it)
	{
		NewEntity * ent = GetEntity(*it);
		if (ent!=0) {
			PositionComponent * p = ent->getComponent<PositionComponent>();
			if (p!=0) {
				med += p->p;
				++n;
			}
		}
	}
	GlobalPosition med2(med);
	med2.x = n;
	med2.y = n;
	med2.z = n;
	med2.local_pos /= n;
	med2 += Vec3(((float)med.x)/n-med2.x, ((float)med.y)/n-med2.y, ((float)med.z)/n-med2.z)*GP_SCALE;
	return med2;
}

void World::SphereCheck(const GlobalPosition& vP, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	/*size_t start;
	for (int i=0;i<units.size();++i) {
		CollisionMesh * solid = dynamic_cast<CollisionMesh*>(units[i]);
		if (solid!=0) {
			start = list.size();
			solid->SphereCheck(vP-units[i]->p, r, list);
			for (int j=start;j<list.size();++j) {
				list[j]->entity = units[i];
				list[j]->poc += units[i]->p;
				list[j]->poo += units[i]->p;
			}
		}
	}*/
	/*for (int i=0;i<units.size();++i) {
		Unit * unit = dynamic_cast<Unit*>(units[i]);
		if (unit!=0) {
			float d = (vP-unit->p).Len();
			if (d <= r + unit->getSize() && d!=0.0f) {
				Collision * col = new Collision();
				col->entity = unit;
				col->n = (vP-unit->p)/d;
				col->disp = col->n*(r+unit->getSize()-d);
				col->dist = d-unit->getSize();
				col->p = unit->p + col->n*unit->getSize();
				list.push_back(col);
			}
		}
	}*/
}

void World::LineCheck(const GlobalPosition& sP, const GlobalPosition& eP, std::vector<std::shared_ptr<Collision>>& list)
{
	/*size_t start;
	for (int i=0;i<units.size();++i) {
		CollisionMesh * solid = dynamic_cast<CollisionMesh*>(units[i]);
		if (solid!=0) {
			start = list.size();
			solid->LineCheck(sP-units[i]->p, eP-units[i]->p, list);
			for (int j=start;j<list.size();++j) {
				list[j]->entity = units[i];
				list[j]->poc += units[i]->p;
				list[j]->poo += units[i]->p;
			}
		}
	}*/
	/*for (int i=0;i<units.size();++i) {
		Unit * unit = dynamic_cast<Unit*>(units[i]);
		if (unit!=0) {
			Vec3 l = eP-sP;
			float len = l.Len();
			l /= len;
			float sqd = (l.Dot(unit->p-sP));
			sqd *= sqd;
			sqd -= (sP-unit->p).LenPwr();
			sqd += unit->getSize()*unit->getSize();
			if (sqd >= 0.0f) {
				sqd = sqrt(sqd);
				float d = (l.Dot(unit->p-sP));
				float d_close = d-sqd;
				float d_far = d+sqd;
				if (d_close <= 1.0f && d_far >= 0.0f) {
					Collision * col = new Collision();
					col->entity = unit;
					col->p = sP + l * d_close;
					col->n = (col->p-unit->p);
					col->n /= col->n.Len();
					col->disp = col->p - eP;
					col->dist = d_close*len;
					list.push_back(col);
				}
			}
		}
	}*/
}

void World::SphereCast(const GlobalPosition& sP, const GlobalPosition& eP, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	//size_t start;
	//for (int i=0;i<units.size();++i) {
	//	CollisionMesh * solid = dynamic_cast<CollisionMesh*>(units[i]);
	//	if (solid!=0) {
	//		start = list.size();
	////LARGE_INTEGER freq, strat, end;
	////QueryPerformanceFrequency(&freq);
	////QueryPerformanceCounter(&strat);
	//		solid->SphereCast(sP-units[i]->p, eP-units[i]->p, r, list);
	////QueryPerformanceCounter(&end);
	////double durationInSeconds = static_cast<double>(end.QuadPart - strat.QuadPart) / freq.QuadPart;
	////Profiler::add(durationInSeconds);
	//		for (int j=start;j<list.size();++j) {
	//			list[j]->entity = units[i];
	//			list[j]->poc += units[i]->p;
	//			list[j]->poo += units[i]->p;
	//		}
	//	}
	//}
}

void World::UnitCheck(const GlobalPosition& vP, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	//for (int i=0;i<units.size();++i) {
	//	Unit * unit = 0;//dynamic_cast<Unit*>(units[i]);
	//	if (unit!=0) {
	//		Vec3 dif = vP-unit->p;
	//		float d = dif.Len();
	//		if (d <= r + unit->r && d!=0.0f) {
	//			auto col = std::make_shared<Collision>();
	//			col->entity = unit;
	//			col->n = dif/d;
	//			col->disp = col->n*(r+unit->r-d);
	//			col->dist = d;
	//			col->poc = unit->p + Vec3(0.0f, 0.0f, unit->r) + col->n*unit->r;
	//			list.push_back(col);
	//		}
	//	}
	//}
}

void World::UnitCheck(const GlobalPosition& vP, const Matrix3& mtrx, std::vector<std::shared_ptr<Collision>>& list)
{
	//Matrix3 inv = mtrx.Inverse();
	//for (int i=0;i<units.size();++i) {
	//	Unit * unit = 0;//dynamic_cast<Unit*>(units[i]);
	//	if (unit!=0) {
	//		Vec3 dif = vP-unit->p;
	//		float l = dif.Len();
	//		dif -= dif*unit->r/l;
	//		dif *= inv;
	//		float d = dif.Len();
	//		if (d <= 1.0f && d!=0.0f) {
	//			auto col = std::make_shared<Collision>();
	//			col->entity = unit;
	//			col->n = dif/d;
	//			//col->disp = col->n*();
	//			//col->dist = d-unit->getSize();
	//			//col->p = unit->p + col->n*unit->getSize();
	//			list.push_back(col);
	//		}
	//	}
	//}
}