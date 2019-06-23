#include "MissileEffect.h"

#include "MissileComponent.h"

MissileEffect::MissileEffect(uint32_t index) : missile_index(index)
{
}

MissileEffect::~MissileEffect()
{
}

void MissileEffect::apply(const AbilityContext& ac) const
{
	MissileComponent::create(missile_index, ac);
}