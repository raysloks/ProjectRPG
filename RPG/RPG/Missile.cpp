#include "Missile.h"

void MissileComponent::tick(float dTime)
{
	auto ent = entity->world->GetEntity(target_mob_id);
	if (ent)
	{
		auto mob = ent->getComponent<MobComponent>();
		if (mob)
		{
			Vec3 diff = mob->p->p - target;
			target += diff * (1.0f - exp(log(remaining) * dTime));
		}
	}
}