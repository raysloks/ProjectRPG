#include "AIComponent.h"

#include "NewEntity.h"
#include "World.h"
#include "MobComponent.h"

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

void AIComponent::frame(float dTime)
{
}

void AIComponent::tick(float dTime)
{
	if (mob == nullptr)
	{
		mob = entity->getComponent<MobComponent>();
	}

	if (mob != nullptr)
	{
		std::uniform_real_distribution<float> simple_distribution;
		if (simple_distribution(random) > std::powf(0.5f, dTime))
		{
			std::uniform_real_distribution<float> angle_distribution(0.0f, M_PI * 2.0f);

			float angle = angle_distribution(random);
			wander += Vec3(cos(angle), sin(angle), 0.0f) * 1.1f;
			wander.Normalize();
		}

		mob->move = wander * 0.5f;
		mob->strafe = false;

		if (mob->p != nullptr)
		{
			auto nearby_mobs = entity->world->GetNearestComponents<MobComponent>(*mob->p, 10.0f);

			MobComponent * other = nullptr;

			for (auto i = nearby_mobs.begin(); i != nearby_mobs.end(); ++i)
			{
				if (i->second != mob)
				{
					other = i->second;
					break;
				}
			}

			if (other != nullptr)
			{
				charge += dTime;

				float distance = Vec3(*other->p - *mob->p).Len();
				
				if (charge > 1.0f)
					mob->move = *other->p - *mob->p;

				if (charge > 2.0f) {
					mob->input["attack"] = 1.0f;
					charge = 0.0f;
				}

				mob->strafe = true;
				mob->cam_facing = Vec3(*other->p - *mob->p).Normalize();
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