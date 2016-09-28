#include "HitComponent.h"
#include "Wall.h"
#include "PositionComponent.h"
#include "MobComponent.h"
#include "NewEntity.h"
#include "World.h"

const AutoSerialFactory<HitComponent> HitComponent::_factory("HitComponent");

HitComponent::HitComponent(void) : Serializable(_factory.id)
{
	points.push_back(Vec3(0.0f, 0.5f, 0.0f));
	points.push_back(Vec3(0.0f, 1.0f, 0.0f));
	points.push_back(Vec3(0.0f, 1.5f, 0.0f));
	points.push_back(Vec3(0.0f, 2.0f, 0.0f));
	points.push_back(Vec3(0.0f, 2.5f, 0.0f));
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

void HitComponent::frame(float dTime)
{
}

void HitComponent::tick(float dTime)
{
	GlobalPosition p;
	PositionComponent * pc = entity->getComponent<PositionComponent>();
	if (pc != 0)
		p = pc->p;

	if (active)
	{
		for (auto ent = entity->world->units.begin(); ent != entity->world->units.end(); ++ent)
		{
			if (*ent != 0)
			{
				MobComponent * mc = (*ent)->getComponent<MobComponent>();
				if (mc != 0 && hit.find(mc) == hit.end())
				{
					if (mc->p != 0)
					{
						GlobalPosition target = *mc->p + mc->up * 0.5f;
						GlobalPosition prev_target = target;
						float r = 0.5f;

						for (auto i = points.begin(); i != points.end(); ++i)
						{
							auto c = Wall::SphereLine(*i*prev_mtrx + (prev_p - prev_target), *i*mtrx + (p - target), Vec3(), r);

							if (c != 0 || Vec3((prev_p + *i*prev_mtrx) - prev_target).LenPwr() <= r*r)
							{
								mc->hit = true;
								hit.insert(mc);
								//std::cout << "hit " << std::distance(points.begin(), i) << " on " << std::distance(entity->world->units.begin(), ent) << std::endl;
							}
						}
					}
				}
			}
		}
	}
	prev_p = p;
	prev_mtrx = mtrx;
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