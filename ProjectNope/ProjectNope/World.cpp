#include "Server.h"
#include "Client.h"
#include "GUIObject.h"
#include <iostream>
#include "streams.h"
#include <fstream>

#include "Profiler.h"

#include "PositionComponent.h"

#include "World.h"

#include "ClientData.h"

World::World(void)
{
	authority = true;
}

World::~World(void)
{
	clear();
}

void World::pre_frame(float dTime)
{
	TimeslotA(pre_frame);

	for (size_t i = 0; i < units.size(); i++)
	{
		if (units[i] != nullptr)
			units[i]->pre_frame(dTime);
	}
}

void World::post_frame(float dTime)
{
	TimeslotA(post_frame);

	for (size_t i = 0; i < units.size(); i++)
	{
		if (units[i] != nullptr)
			units[i]->post_frame(dTime);
	}
}

void World::tick(float dTime)
{
	TimeslotA(tick);

	for (size_t i = 0; i < units.size(); i++)
	{
		if (units[i] != nullptr)
			units[i]->tick(dTime);
	}
}

void World::clean(void)
{
	for (auto i = removed.begin(); i != removed.end(); ++i)
		delete *i;
	removed.clear();
}

void World::clear(void)
{
	for (size_t i = 0; i < units.size(); i++)
	{
		SetEntity(i, nullptr);
	}
}

#include "GraphicsComponent.h"

#include "RenderSetup.h"

void World::render(RenderSetup& rs)
{
	rs.origin = cam_pos;

	GraphicsComponent::prep(rs);
	GraphicsComponent::render_all(rs);

	// TODO component rendering
	/*for (int i=0;i<units.size();++i) {
		if (units[i]!=0)
			units[i]->render(cam_pos);
	}*/
}

uint32_t World::AddEntity(NewEntity * unit)
{
	if (unit != nullptr && authority) {
		uint32_t id = 0xffffffff;
		while (id == 0xffffffff)
		{
			if (alloc.size()) {
				int i = alloc.top();
				alloc.pop();
				if (units[i] == nullptr) {
					id = i;
				}
			} else {
				break;
			}
		}
		if (id == 0xffffffff)
		{
			for (size_t i = 0; i < units.size(); i++)
			{
				if (units[i] == nullptr)
				{
					id = i;
					break;
				}
			}
		}
		if (id == 0xffffffff)
		{
			id = units.size();
			unit->id = id;
			unit->world = this;
			units.push_back(unit);
			uid.push_back(0);
			if (server)
				server->NotifyOfCreation(id);
		}
		else
		{
			unit->id = id;
			unit->world = this;
			units[id] = unit;
			if (server)
				server->NotifyOfCreation(id);
		}
		return id;
	}
	return 0xffffffff;
}

void World::SetEntity(uint32_t id, NewEntity * unit)
{
	if (id == 0xffffffff)
		return;
	if (id >= units.size())
	{
		units.resize(id + 1);
		uid.resize(id + 1);
	}
	if (units[id] != nullptr)
	{
		if (server != nullptr && authority)
			server->NotifyOfRemoval(id, uid[id]);
		removed.push_back(units[id]);
		if (unit == nullptr)
			alloc.push(id);
	}
	if (unit != nullptr)
	{
		unit->id = id;
		unit->world = this;
		uid[id]++;
	}
	units[id] = unit;
}

NewEntity * World::GetEntity(uint32_t id)
{
	if (id >= units.size())
		return nullptr;
	return units[id];
}

NewEntity * World::GetEntity(uint32_t id, uint32_t unid)
{
	if (id >= units.size())
		return nullptr;
	if (units[id] != nullptr)
		if (uid[id] != unid)
			return nullptr;
	return units[id];
}

NewEntity * World::GetEntity(EntityID id)
{
	return GetEntity(id.id, id.uid);
}

std::multimap<float, NewEntity*> World::GetNearestEntities(const GlobalPosition& p)
{
	TimeslotC(get_nearest_entities);

	std::multimap<float, NewEntity*> ret;

	for (auto i = units.begin(); i != units.end(); ++i)
	{
		if (*i != nullptr)
		{
			auto pc = (*i)->getComponent<PositionComponent>();
			if (pc != nullptr)
			{
				float distance = Vec3(p - pc->p).Len();
				ret.insert(std::make_pair(distance, *i));
			}
		}
	}

	return ret;
}

std::multimap<float, NewEntity*> World::GetNearestEntities(const GlobalPosition& p, float r)
{
	TimeslotC(get_nearest_entities);

	std::multimap<float, NewEntity*> ret;

	for (auto i = units.begin(); i != units.end(); ++i)
	{
		if (*i != nullptr)
		{
			auto pc = (*i)->getComponent<PositionComponent>();
			if (pc != nullptr)
			{
				float distance = Vec3(p - pc->p).Len();
				if (distance <= r)
				{
					ret.insert(std::make_pair(distance, *i));
				}
			}
		}
	}

	return ret;
}

GlobalPosition World::getMed(const std::set<uint32_t>& e)
{
	std::set<uint32_t> ents = e;
	GlobalPosition med;
	int n = 0;
	for (auto it = ents.begin(); it != ents.end(); ++it)
	{
		auto ent = GetEntity(*it);
		if (ent)
		{
			auto p = ent->getComponent<PositionComponent>();
			if (p)
			{
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