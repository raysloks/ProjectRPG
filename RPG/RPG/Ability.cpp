#include "Ability.h"

#include "AbilityContext.h"
#include "Effect.h"

#include "MobComponent.h"
#include "AnimationControlComponent.h"

#include "NewEntity.h"

#include "SimpleState.h"

Ability::Ability()
{
}

Ability::~Ability()
{
}

void Ability::activate(const AbilityContext& ac) const
{
	auto finish = [=]
	{
		for (auto effect : effects)
			effect->apply(ac);
	};
	if (cast_time > 0.0f)
	{
		auto acc = ac.source->entity->getComponent<AnimationControlComponent>();
		auto cast = new SimpleState("cast", 1.0f / cast_time);
		cast->events.insert(std::make_pair(1.0f, finish));
	}
	else
	{
		finish();
	}
}

std::vector<Ability> abilities;

void Ability::init()
{
	abilities.reserve(4096);

	abilities.resize(256);

	abilities[0].cast_time = 3.0f;
	abilities[0].icon = "data/assets/y.tga";

	abilities[1].cast_time = 3.0f;
	abilities[1].icon = "data/assets/wibbly.tga";
}

const Ability * Ability::get(uint32_t index)
{
	if (index < abilities.size())
		return &abilities[index];
	return nullptr;
}

void Ability::release()
{
	abilities.clear();
}