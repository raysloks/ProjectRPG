#include "AIComponent.h"

#include "NewEntity.h"
#include "World.h"

#include "MobComponent.h"
#include "ColliderComponent.h"
#include "AnimationControlComponent.h"
#include "ProjectileComponent.h"
#include "GraphicsComponent.h"

#include "SimpleState.h"

const AutoSerialFactory<AIComponent> AIComponent::_factory("AIComponent");

AIComponent::AIComponent(void) : Serializable(_factory.id)
{
}

AIComponent::AIComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

AIComponent::~AIComponent(void)
{
}

void AIComponent::connect(NewEntity * pEntity, bool authority)
{
}

void AIComponent::disconnect(void)
{
}

void AIComponent::pre_frame(float dTime)
{
}

void AIComponent::tick(float dTime)
{
	if (entity->world->authority)
	{
		auto mob = entity->getComponent<MobComponent>();
		auto p = entity->getComponent<PositionComponent>();
		auto acc = entity->getComponent<AnimationControlComponent>();

		if (mob && p && acc)
		{
			auto target_entity = entity->world->GetEntity(target);
			if (target_entity)
			{
				auto target_p = target_entity->getComponent<PositionComponent>();
				if (target_p)
				{
					Vec3 dif = target_p->p - p->p;
					float distance = dif.Len();
					Vec3 dir = dif / distance;
					mob->move = dir;
					float facing_dot_dir = mob->facing.Dot(dir);
					if (distance < 3.0f && facing_dot_dir > 0.8f && acc->has_state("run") && delay > 0.2f)
					{
						acc->set_state(new SimpleState("attack_down", 1.0f));
						mob->stamina.current -= 1;
						delay = 0.0f;
					}
					if (distance < 5.0f && facing_dot_dir > 0.8f && acc->has_state("run") && delay > 0.4f)
					{
						acc->set_state(new SimpleState("attack_sweep", 0.8f));
						mob->stamina.current -= 1;
						delay = 0.0f;
					}
					if (distance < 7.0f && facing_dot_dir > 0.8f && acc->has_state("run") && delay > 0.6f)
					{
						acc->set_state(new SimpleState("attack_heavy", 0.3f));
						mob->stamina.current -= 2;
						delay = 0.0f;
					}
					if (mob->mana.current > 0 && facing_dot_dir > 0.8f && acc->has_state("run") && delay > 0.8f)
					{
						uint32_t power = std::min(5, mob->mana.current);
						auto state = new SimpleState("attack_chest", 0.7f);
						state->events.insert(std::make_pair(0.5f, [=]()
						{
							mob->mana.current -= power;
							
							NewEntity * ent = new NewEntity();

							PositionComponent * pos = new PositionComponent();
							ProjectileComponent * projectile = new ProjectileComponent();
							GraphicsComponent * g = new GraphicsComponent(false);

							ent->addComponent(pos);
							ent->addComponent(projectile);
							ent->addComponent(g);

							projectile->v = dif + mob->v + Vec3(0.0f, 0.0f, 9.8f / 2.0f);

							pos->p = *mob->p;

							projectile->on_collision = [=](MobComponent * target)
							{
								if (target)
								{
									target->do_damage(5, mob->entity->get_id());
									target->hit = true;
								}
								ent->world->SetEntity(ent->id, nullptr);
							};

							projectile->shooter = mob;

							g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cube.gmdl", Material("data/assets/black.tga"), 0)));

							entity->world->AddEntity(ent);
						}));
						acc->set_state(state);
						delay = 0.0f;
					}

					if (acc->has_state("run"))
						delay += dTime;
				}
			}
			else
			{
				auto nearby = entity->world->GetNearestComponents<MobComponent>(p->p, 30.0f);
				for (auto other : nearby)
				{
					if (other.second->temp_team != mob->temp_team)
						target = other.second->entity->get_id();
				}
			}
		}
	}
}

void AIComponent::writeLog(outstream& os, ClientData& client)
{
}

void AIComponent::readLog(instream& is)
{
}

void AIComponent::writeLog(outstream& os)
{
}

void AIComponent::readLog(instream& is, ClientData& client)
{
}

void AIComponent::interpolate(Component * pComponent, float fWeight)
{
}

void AIComponent::write_to(outstream& os, ClientData& client) const
{
}

void AIComponent::write_to(outstream& os) const
{
}