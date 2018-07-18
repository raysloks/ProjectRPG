#include "AIComponent.h"

#include "NewEntity.h"
#include "World.h"
#include "MobComponent.h"
#include "ColliderComponent.h"
#include "AnimationControlComponent.h"

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
					if (distance < 5.0f && facing_dot_dir > 0.8f && !acc->has_state("attack"))
					{
						acc->set_state(new SimpleState("attack", 0.25f));
						mob->stamina.current -= 2;
					}
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