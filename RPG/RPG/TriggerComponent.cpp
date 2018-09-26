#include "TriggerComponent.h"

#include "NewEntity.h"
#include "World.h"

#include "MobComponent.h"

const AutoSerialFactory<TriggerComponent> TriggerComponent::_factory("TriggerComponent");

TriggerComponent::TriggerComponent(void) : Serializable(_factory.id)
{
}

TriggerComponent::TriggerComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

TriggerComponent::~TriggerComponent(void)
{
}

void TriggerComponent::connect(NewEntity * pEntity, bool authority)
{
}

void TriggerComponent::disconnect(void)
{
}

void TriggerComponent::pre_frame(float dTime)
{
}

void TriggerComponent::post_frame(float dTime)
{
}

void TriggerComponent::tick(float dTime)
{
	countdown -= dTime;
	if (countdown <= 0.0f)
	{
		countdown += delay;
		auto mobs = entity->world->GetComponents<MobComponent>();
		std::vector<MobComponent*> passed_mobs;
		for each (auto mob in mobs)
		{
			if (mob->team == 0)
			{
				auto p = mob->entity->getComponent<PositionComponent>();
				if (p)
				{
					if (inside_area(p->p))
					{
						if (func)
							func(mob);
						passed_mobs.push_back(mob);
					}
				}
			}
		}
		if (func_multi && passed_mobs.size())
			func_multi(passed_mobs);
	}
}

void TriggerComponent::writeLog(outstream& os, ClientData& client)
{
}

void TriggerComponent::readLog(instream& is)
{
}

void TriggerComponent::writeLog(outstream& os)
{
}

void TriggerComponent::readLog(instream& is, ClientData& client)
{
}

void TriggerComponent::interpolate(Component * pComponent, float fWeight)
{
}

void TriggerComponent::write_to(outstream& os, ClientData& client) const
{
}

void TriggerComponent::write_to(outstream& os) const
{
}

bool TriggerComponent::inside_area(const GlobalPosition& p) const
{
	Vec3 dif_min = p - aabb_min;
	Vec3 dif_max = p - aabb_max;
	return dif_min > Vec3() && dif_max < Vec3();
}
