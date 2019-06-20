#include "ColliderComponent.h"

#include <algorithm>

#include "NewEntity.h"
#include "World.h"

#include "PositionComponent.h"
#include "GraphicsComponent.h"

AutoSerialFactory<ColliderComponent, Component> ColliderComponent::_factory("ColliderComponent");

thread_local std::vector<ColliderComponent*> ColliderComponent::all;

ColliderComponent::ColliderComponent(bool d) : Component(_factory.id), deform(d)
{
	all.push_back(this);
}

ColliderComponent::ColliderComponent(instream& is) : Component(_factory.id)
{
	all.push_back(this);
	is >> deform;
}

ColliderComponent::~ColliderComponent(void)
{
	all.erase(std::remove(all.begin(), all.end(), this), all.end());
}

void ColliderComponent::connect(NewEntity * pEntity, bool authority)
{
}

void ColliderComponent::disconnect(void)
{
}

void ColliderComponent::tick(float dTime)
{
	auto pc = entity->getComponent<PositionComponent>();
	if (pc)
	{
		update(pc->p, dTime);
	}
	if (mesh.walls.size()==0 || deform)
	{
		auto gc = entity->getComponent<GraphicsComponent>();
		if (gc != 0)
		{
			gc->pre_frame(0.0f);
			if (gc->decs.items.size() != 0)
			{
				if (gc->decs.items.front()->skin != 0)
				{
					mesh.Recalculate(*gc->decs.items.front()->skin, *gc->decs.items.front()->skin, gc->decs.items.front()->local, gc->decs.items.front()->local, dTime);
				}
			}
		}
	}
}

void ColliderComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
}

void ColliderComponent::readLog(instream& is)
{
}

void ColliderComponent::writeLog(outstream& os)
{
}

void ColliderComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
}

void ColliderComponent::interpolate(Component * pComponent, float fWeight)
{
}

void ColliderComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
	os << deform;
}

void ColliderComponent::write_to(outstream& os) const
{
	os << deform;
}

void ColliderComponent::update(const GlobalPosition& next, float dTime)
{
	prev = p;
	p = next;
	v = Vec3(p - prev)/dTime;
}

#include "Profiler.h"
#include "GUIObject.h"

void ColliderComponent::LineCheck(const GlobalPosition& sP, const GlobalPosition& eP, std::vector<std::shared_ptr<Collision>>& list)
{
	TimeslotB(collision);

	for (auto i = all.begin(); i != all.end(); ++i) {
		if (*i) {
			int j = list.size();
			(*i)->mesh.LineCheck(sP - (*i)->prev, eP - (*i)->p, list);
			for (; j<list.size(); ++j) {
				list[j]->comp = *i;
				list[j]->poc += (*i)->p;
				list[j]->poo += (*i)->p;
				list[j]->v += (*i)->v;
			}
		}
	}
}

void ColliderComponent::SphereCheck(const GlobalPosition& vP, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	TimeslotB(collision);

	for (auto i = all.begin(); i != all.end(); ++i) {
		if (*i) {
			int j = list.size();
			(*i)->mesh.SphereCheck(vP - (*i)->prev/*-v*dTime*/, r, list);
			for (; j<list.size(); ++j) {
				list[j]->comp = *i;
				list[j]->poc += (*i)->prev;
				list[j]->poo += (*i)->prev;
				list[j]->v += (*i)->v;
			}
		}
	}
}

void ColliderComponent::SphereCast(const GlobalPosition& sP, const GlobalPosition& eP, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	TimeslotB(collision);

	for (auto i=all.begin();i!=all.end();++i) {
		if (*i) {
			int j = list.size();
			(*i)->mesh.SphereCast(sP-(*i)->prev, eP-(*i)->p/*-v*dTime*/, r, list);
			Vec3 dif = (*i)->p - (*i)->prev;
			for (;j<list.size();++j) {
				list[j]->comp = *i;
				list[j]->poc += (*i)->prev + dif * list[j]->t;
				list[j]->poo += (*i)->prev + dif * list[j]->t;;
				list[j]->v += (*i)->v;
			}
		}
	}
}

void ColliderComponent::DiskCast(const GlobalPosition& sP, const GlobalPosition& eP, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	TimeslotB(collision);

	for (auto i = all.begin(); i != all.end(); ++i) {
		if (*i) {
			int j = list.size();
			(*i)->mesh.DiskCast(sP - (*i)->prev, eP - (*i)->prev, r, list);
			for (; j<list.size(); ++j) {
				list[j]->comp = *i;
				list[j]->poc += (*i)->prev;
				list[j]->poo += (*i)->prev;
				list[j]->v += (*i)->v;
			}
		}
	}
}

void ColliderComponent::LowerDisk(const Vec3& lock, const Vec3& center, const Vec3& axis, const Vec3& dir, float r, std::vector<std::shared_ptr<Collision>>& list)
{
	for (auto i = all.begin(); i != all.end(); ++i) {
		if (*i) {
			int j = list.size();
			(*i)->mesh.LowerDisk(lock - (*i)->p, center - (*i)->p, axis, dir, r, list);
			for (; j<list.size(); ++j) {
				list[j]->comp = *i;
				list[j]->poc += (*i)->p;
				list[j]->poo += (*i)->p;
				list[j]->v += (*i)->v;
			}
		}
	}
}
