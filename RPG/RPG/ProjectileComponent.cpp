#include "ProjectileComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "PositionComponent.h"
#include "ColliderComponent.h"
#include "MobComponent.h"

const AutoSerialFactory<ProjectileComponent> ProjectileComponent::_factory("ProjectileComponent");

ProjectileComponent::ProjectileComponent(void) : Serializable(_factory.id)
{
}

ProjectileComponent::ProjectileComponent(instream& is, bool full) : Serializable(_factory.id)
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

		auto mobs = entity->world->GetNearestComponents<MobComponent>(pc->p);
		for each (auto mob in mobs)
		{
			auto col = Wall::SphereLine(Vec3(), dp, *mob.second->p - pc->p, 0.5f);
			if (col != nullptr)
			{
				mob.second->health.current -= 20.0f;
				mob.second->hit = true;
				v = mob.second->v;
			}
		}

		std::vector<std::shared_ptr<Collision>> list;
		ColliderComponent::LineCheck(pc->p, pc->p + dp, list);
		if (list.empty())
		{
			pc->p += v * dTime + g * dTime * dTime;
		}
		else
		{
			std::sort(list.begin(), list.end(), [](const std::shared_ptr<Collision>& lhs, const std::shared_ptr<Collision>& rhs) -> bool {
				return lhs->t < rhs->t;
			});
			pc->p = list.front()->poc;
			//entity->removeComponent(this);
			if (v.LenPwr() > 100.0f)
			{
				v -= list.front()->n * list.front()->n.Dot(v) * 2.0f;
				float v_mod = 1.0f - v.Normalized().Dot(list.front()->n);
				v_mod *= v_mod;
				v_mod *= v_mod;
				v *= v_mod;
			}
			else
			{
				entity->world->SetEntity(entity->id, nullptr);
			}
		}

		v += g * dTime;

		v *= exp(-drag * v.Len() * dTime);

		if (pc->p.z < -10)
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