#include "SpawnComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "PositionComponent.h"
#include "MobComponent.h"
#include "ColliderComponent.h"
#include "GraphicsComponent.h"
#include "AIComponent.h"
#include "AnimationControlComponent.h"
#include "PoseComponent.h"
#include "HitComponent.h"

const AutoSerialFactory<SpawnComponent> SpawnComponent::_factory("SpawnComponent");

SpawnComponent::SpawnComponent(void) : Serializable(_factory.id)
{
}

SpawnComponent::SpawnComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

SpawnComponent::~SpawnComponent(void)
{
}

void SpawnComponent::connect(NewEntity * pEntity, bool authority)
{
}

void SpawnComponent::disconnect(void)
{
}

void SpawnComponent::pre_frame(float dTime)
{
}

void SpawnComponent::post_frame(float dTime)
{
}

void SpawnComponent::tick(float dTime)
{
}

void SpawnComponent::writeLog(outstream& os, ClientData& client)
{
}

void SpawnComponent::readLog(instream& is)
{
}

void SpawnComponent::writeLog(outstream& os)
{
}

void SpawnComponent::readLog(instream& is, ClientData& client)
{
}

void SpawnComponent::interpolate(Component * pComponent, float fWeight)
{
}

void SpawnComponent::write_to(outstream& os, ClientData& client) const
{
}

void SpawnComponent::write_to(outstream& os) const
{
}

MobComponent * SpawnComponent::spawn()
{
	return nullptr;
}

MobComponent * SpawnComponent::spawn(const Vec3& v)
{
	return nullptr;
}

GlobalPosition SpawnComponent::select_position(void)
{
	Vec3 dif = aabb_max - aabb_min;
	std::uniform_real_distribution<float> uni_dist;
	auto pos = aabb_min + dif * Vec3(uni_dist(random), uni_dist(random), uni_dist(random));
	std::vector<std::shared_ptr<Collision>> list;
	ColliderComponent::SphereCast(pos, pos + Vec3(0.0f, 0.0f, -10.0f), 0.5f, list);
	if (list.empty())
		return pos;
	std::sort(list.begin(), list.end(), [](const std::shared_ptr<Collision>& a, const std::shared_ptr<Collision>& b) { return a->t < b->t; });
	return list.front()->poo;
}

bool SpawnComponent::is_valid(const GlobalPosition& p) const
{
	return true;
}

bool SpawnComponent::inside_area(const GlobalPosition& p) const
{
	Vec3 dif_min = p - aabb_min;
	Vec3 dif_max = p - aabb_max;
	return dif_min > Vec3() && dif_max < Vec3();
}

bool SpawnComponent::in_view(const GlobalPosition& p, World * world, const std::function<bool(MobComponent*)>& pred)
{
	auto mobs = world->GetNearestComponents<MobComponent>(p);
	bool seen = false;
	for each (auto mob in mobs)
	{
		if (pred(mob.second))
		{
			std::vector<std::shared_ptr<Collision>> list;
			auto pc = mob.second->entity->getComponent<PositionComponent>();
			ColliderComponent::LineCheck(p, pc->p, list);
			if (list.empty())
			{
				seen = true;
			}
		}
	}
	return seen;
}
