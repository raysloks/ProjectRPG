#include "Ability.h"

Ability::Ability()
{
}

Ability::~Ability()
{
}

void Ability::activate(MobComponent * user, MobComponent * target)
{
  auto finish = []
  {
  for (auto factory : user_auras)
    user->attach(factory->create());
  for (auto factory : target_auras)
    target->attach(factory->create());
  };
  if (cast_time > 0.0f)
  {
    auto acc = mob->entity->getComponent<AnimationControlComponent>();
    acc->setState(new CastState(cast_time, interrutability));
    acc->state->events.insert(std::make_pair(1.0f, finish));
  }
  else
  {
    finish();
  }
}