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
			Effect::get(effect)->apply(ac);
	};
	if (cast_time > 0.0f)
	{
		auto acc = ac.source->entity->getComponent<AnimationControlComponent>();
		auto cast = new SimpleState("quaff", 1.0f / cast_time);
		cast->length = cast_time * 100.0f;
		cast->events.insert(std::make_pair(1.0f, finish));
		acc->set_state(cast);
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

	abilities[0].cast_time = 1.0f;
	abilities[0].icon = "data/assets/y.tga";
	abilities[0].name = "Barrier";
	abilities[0].effects.push_back(0);

	abilities[1].cast_time = 3.0f;
	abilities[1].icon = "data/assets/wibbly.tga";
	abilities[1].name = "Fireball";
	abilities[1].effects.push_back(1);
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