#include "AuraEffect.h"

AuraEffect::~AuraEffect()
{
}

void AuraEffect::apply(const AbilityContext& ac)
{
	ac.target->add_aura(aura->create());
}