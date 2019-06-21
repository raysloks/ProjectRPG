#include "Effect.h"

std::vector<Effect*> effects;

void Effect::init()
{
	effects.reserve(4096);

	effects.resize(256);

	effects[0] = new AuraEffect(0);
}

const Effect * Effect::get(uint32_t index)
{
	if (index < effects.size())
		return effects[index];
	return nullptr;
}

void Effect::release()
{
	for (auto effect : effects)
		if (effect)
			delete effect;
	abilities.clear();
}