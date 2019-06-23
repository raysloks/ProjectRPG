#include "AuraEffect.h"

AuraEffect::AuraEffect(uint32_t index) : aura_index(index)
{
}

AuraEffect::~AuraEffect()
{
}

void AuraEffect::apply(const AbilityContext& ac) const
{
	ac.target->add_aura(Aura::create(aura_index));
}