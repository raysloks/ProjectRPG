#include "MissileComponent.h"

#include "NewEntity.h"
#include "World.h"

#include "MobComponent.h"

void MissileComponent::init()
{
}

MissileComponent * MissileComponent::create(uint32_t index, const AbilityContext & ac)
{
	return nullptr;
}

void MissileComponent::release()
{
}

void MissileComponent::tick(float dTime)
{
	auto ent = entity->world->GetEntity(target_mob_id);
	if (ent)
	{
		auto mob = ent->getComponent<MobComponent>();
		if (mob)
		{
			Vec3 diff = mob->p->p - target;
			target += diff * (1.0f - exp(log(0.0f) * dTime));
		}
	}
}