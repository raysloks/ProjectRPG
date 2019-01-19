#include "ProjectileComponent.h"

#include <algorithm>

#include "NewEntity.h"

#include "World.h"

#include "PositionComponent.h"
#include "ColliderComponent.h"
#include "MobComponent.h"

AutoSerialFactory<ProjectileComponent, Component> ProjectileComponent::_factory("ProjectileComponent");

ProjectileComponent::ProjectileComponent(void) : Component(_factory.id)
{
}

ProjectileComponent::ProjectileComponent(instream& is, bool full) : Component(_factory.id)
{
}

ProjectileComponent::~ProjectileComponent(void)
{
}

void ProjectileComponent::connect(NewEntity * pEntity, bool authority)
{
}

void ProjectileComponent::disconnect(void)
{
}

void ProjectileComponent::pre_frame(float dTime)
{
}

void ProjectileComponent::tick(float dTime)
{
	auto pc = entity->getComponent<PositionComponent>();
	if (pc != nullptr && entity->world->authority)
	{
		Vec3 g = Vec3(0.0f, 0.0f, -1.0f) * 9.8f;

		Vec3 dp = v * dTime + g * dTime * dTime;

		std::vector<std::pair<MobComponent*, std::shared_ptr<Collision>>> hits;
		auto mobs = entity->world->GetNearestComponents<MobComponent>(pc->p);
		for each (auto mob in mobs)
		{
			bool ok = true;
			if (t <= 0.1f)
				if (mob.second == shooter)
					ok = false;
			if (ok)
			{
				auto col = Wall::SphereLine(Vec3(), dp, mob.second->p->p - pc->p, 0.5f);
				if (col != nullptr)
				{
					col->poo += pc->p;
					hits.push_back(std::make_pair(mob.second, col));
				}
			}
		}

		std::vector<std::shared_ptr<Collision>> list;
		ColliderComponent::LineCheck(pc->p, pc->p + dp, list);
		for each (auto col in list)
		{
			hits.push_back(std::make_pair(nullptr, col));
		}

		if (hits.empty())
		{
			t += dTime;
			pc->p += v * dTime + g * dTime * dTime;
		}
		else
		{
			std::sort(list.begin(), list.end(), [](const std::shared_ptr<Collision>& lhs, const std::shared_ptr<Collision>& rhs) {
				return lhs->t < rhs->t;
			});
			t += hits.front().second->t;
			pc->p = hits.front().second->poo;
			if (on_collision)
				on_collision(hits.front().first);
		}

		v += g * dTime;

		v *= exp(-drag * v.Len() * dTime);

		if (pc->p.z < -250.0f)
			entity->world->SetEntity(entity->id, nullptr);

		pc->update();
	}
}

void ProjectileComponent::writeLog(outstream& os, ClientData& client)
{
}

void ProjectileComponent::readLog(instream& is)
{
}

void ProjectileComponent::writeLog(outstream& os)
{
}

void ProjectileComponent::readLog(instream& is, ClientData& client)
{
}

void ProjectileComponent::interpolate(Component * pComponent, float fWeight)
{
}

void ProjectileComponent::write_to(outstream& os, ClientData& client) const
{
}

void ProjectileComponent::write_to(outstream& os) const
{
}