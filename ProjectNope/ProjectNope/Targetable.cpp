#include "Targetable.h"
#include <set>

#include "TestServer.h"
#include "World.h"

extern TestServer * server;

const AutoSerialFactory<Targetable> Targetable::_factory_Targetable("Targetable");

Targetable::Targetable(const GlobalPosition& pos, const Vec3& vel, float size) : PNEntity(pos), v(vel), r(size), Serializable(Targetable::_factory_Targetable.id)
{
}

Targetable::Targetable(instream& is, bool full) : PNEntity(is, full), Serializable(Targetable::_factory_Targetable.id)
{
	is >> r;
}

Targetable::~Targetable(void)
{
}

void Targetable::tick(float dTime)
{
	if (world->authority)
	{
		set_changed();

		Vec3 target_move = v*dTime + move;
		move = Vec3();//can be moved elsewhere?
		Vec3 current_move = target_move;
		float t = 1.0f;
		std::set<void*> ignored;
		std::set<void*> hit;
		std::set<Vec3> normals, moves;
		while (t>0.0f)
		{
			float time = t;
			std::vector<std::shared_ptr<Collision>> list;
			world->SphereCast(p, p+current_move, r, list);
			std::shared_ptr<Collision> closest;
			for (auto i=list.begin();i!=list.end();++i)
			{
				if ((*i)->n.Dot(current_move-(*i)->v*dTime)<0.0f) {
					if ((*i)->t<=time) {
						if (ignored.find((*i)->ce)==ignored.end()) {
							time = (*i)->t;
							closest = *i;
						}
					}
				}
			}
			if (closest!=0) {
				if (time>0.0f)
					t -= time;
				OnCollision(closest, dTime);
				p = closest->poo;
				if (closest->n.Dot(v-closest->v)<0.0f)
					v -= closest->n*closest->n.Dot(v-closest->v);
				current_move = v*dTime*t;
				bool should_break = false;
				for (auto i=normals.begin();i!=normals.end();++i) {
					if (i->Dot(closest->n)<=0.0f)
						should_break=true;
				}
				for (auto i=moves.begin();i!=moves.end();++i) {
					if (i->Dot(current_move)<=0.0f)
						should_break=true;
				}
				if (should_break)
					break;
				hit.insert(closest->ce);
				ignored.insert(closest->ce);
			} else {
				if (world->authority) {
					p += current_move;
				}
				break;
			}
		}
	}
}

void Targetable::render(const GlobalPosition& origin)
{
}

void Targetable::interpolate(Entity * pEntity, float fWeight)
{
	Entity::interpolate(pEntity, fWeight);
	Targetable * pTargetable = dynamic_cast<Targetable*>(pEntity);
	if (pTargetable!=0) {
		v *= 1.0f-fWeight;
		v += pTargetable->v*fWeight;
	}
}

void Targetable::write_to(outstream& os, bool full) const
{
	PNEntity::write_to(os, full);
	os << r;
}

void Targetable::OnCollision(const std::shared_ptr<Collision>& col, float dTime)
{
	if (col->n.Dot(v-col->v)<0)
		v += col->n*col->n.Dot(col->v-v);
}