#include "ColliderComponent.h"

#include <algorithm>

#include "NewEntity.h"
#include "World.h"

#include "PositionComponent.h"
#include "GraphicsComponent.h"

const AutoSerialFactory<ColliderComponent> ColliderComponent::_factory("ColliderComponent");

thread_local std::vector<ColliderComponent*> ColliderComponent::all;

ColliderComponent::ColliderComponent(bool d) : Serializable(_factory.id), deform(d)
{
	all.push_back(this);
}

ColliderComponent::ColliderComponent(instream& is, bool full) : Serializable(_factory.id)
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
	if (pc!=0)
		update(pc->p, dTime);
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
					mesh.Recalculate(*gc->decs.items.front()->skin, *gc->decs.items.front()->skin, gc->decs.items.front()->final, gc->decs.items.front()->local, dTime);
				}
			}
		}
	}
}

void ColliderComponent::writeLog(outstream& os, ClientData& client)
{
}

void ColliderComponent::readLog(instream& is)
{
}

void ColliderComponent::writeLog(outstream& os)
{
}

void ColliderComponent::readLog(instream& is, ClientData& client)
{
}

void ColliderComponent::interpolate(Component * pComponent, float fWeight)
{
}

void ColliderComponent::write_to(outstream& os, ClientData& client) const
{
	os << deform;
}

void ColliderComponent::write_to(outstream& os) const
{
	os << deform;
}

void ColliderComponent::update(const GlobalPosition& next_position, float dTime)
{
	p = next;
	next = next_position;
	v = Vec3(next-p)/dTime;
}

#include "Profiler.h"
#include "GUIObject.h"

void ColliderComponent::LineCheck(const GlobalPosition& sP, const GlobalPosition& eP, std::vector<std::shared_ptr<Collision>>& list)
{
	TimeslotB(collision);

	for (auto i = all.begin(); i != all.end(); ++i) {
		if (*i) {
			int j = list.size();
			(*i)->mesh.LineCheck(sP - (*i)->p, eP - (*i)->p, list);
			for (; j<list.size(); ++j) {
				list[j]->comp = *i;
				list[j]->poc += (*i)->p;
				list[j]->poo += (*i)->p;
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
			(*i)->mesh.SphereCast(sP-(*i)->p, eP-(*i)->p/*-v*dTime*/, r, list);
			for (;j<list.size();++j) {
				list[j]->comp = *i;
				list[j]->poc += (*i)->p;
				list[j]->poo += (*i)->p;
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
			(*i)->mesh.DiskCast(sP - (*i)->p, eP - (*i)->p, r, list);
			for (; j<list.size(); ++j) {
				list[j]->comp = *i;
				list[j]->poc += (*i)->p;
				list[j]->poo += (*i)->p;
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
