#include "HitComponent.h"
#include "Wall.h"
#include "PositionComponent.h"
#include "MobComponent.h"
#include "NewEntity.h"
#include "World.h"

const AutoSerialFactory<HitComponent> HitComponent::_factory("HitComponent");

HitComponent::HitComponent(void) : Serializable(_factory.id)
{
}

HitComponent::HitComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

HitComponent::~HitComponent(void)
{
}

void HitComponent::connect(NewEntity * pEntity, bool authority)
{
}

void HitComponent::disconnect(void)
{
}

void HitComponent::pre_frame(float dTime)
{
}

void HitComponent::tick(float dTime)
{
	GlobalPosition p;
	PositionComponent * pc = entity->getComponent<PositionComponent>();
	if (pc != 0)
		p = pc->p;

	std::set<MobComponent*> next_hit;

	if (active)
	{
		for (auto ent : entity->world->units)
		{
			if (ent)
			{
				MobComponent * mob = ent->getComponent<MobComponent>();
				if (mob)
				{
					auto mob_p = ent->getComponent<PositionComponent>();
					if (mob_p)
					{
						GlobalPosition target = mob_p->p;
						GlobalPosition prev_target = target;
						float r_plus_r = r + 0.5f;

						auto c = Wall::SphereLine(prev_p - prev_target, p - target, Vec3(), r_plus_r);

						if (c || Vec3(prev_p - prev_target).LenPwr() <= r_plus_r * r_plus_r)
						{
							next_hit.insert(mob);
							if (hit.find(mob) == hit.end())
							{
								if (func)
									func(mob, Vec3(p - prev_p) / dTime);
							}
						}
					}
				}
			}
		}
	}

	hit = next_hit;
	prev_p = p;
}

void HitComponent::writeLog(outstream& os, ClientData& client)
{
}

void HitComponent::readLog(instream& is)
{
}

void HitComponent::writeLog(outstream& os)
{
}

void HitComponent::readLog(instream& is, ClientData& client)
{
}

void HitComponent::interpolate(Component * pComponent, float fWeight)
{
}

void HitComponent::write_to(outstream& os, ClientData& client) const
{
}

void HitComponent::write_to(outstream& os) const
{
}