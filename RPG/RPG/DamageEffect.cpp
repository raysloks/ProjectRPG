#include "DamageEffect.h"

#include "MobComponent.h"
#include "NewEntity.h"

DamageEffect::DamageEffect(int32_t amount) : damage(amount)
{
}

DamageEffect::~DamageEffect()
{
}

void DamageEffect::apply(const AbilityContext& ac) const
{
	ac.target->do_damage(HitData(damage, ac.source->entity->get_id()));
}
