#include "AIComponent.h"

#include "NewEntity.h"
#include "World.h"
#include "MobComponent.h"
#include "ColliderComponent.h"

const AutoSerialFactory<AIComponent> AIComponent::_factory("AIComponent");

AIComponent::AIComponent(void) : Serializable(_factory.id)
{
	idle = [this](float time_over)
	{
		std::uniform_real_distribution<float> uni_dist;

		if (uni_dist(random) > 0.75f)
		{
			std::uniform_real_distribution<float> angle_distribution(0.0f, M_PI * 2.0f);

			float angle = angle_distribution(random);
			mob->move = mob->cam_facing + Vec3(cos(angle), sin(angle), 0.0f) * 1.1f;
			mob->move.Normalize();
			mob->cam_facing = mob->move;
			mob->move *= 0.25f;
		}
		else
		{
			mob->move = Vec3();
		}

		checks.insert(std::make_pair((uni_dist(random) + uni_dist(random)) * 4.0f + time_over, idle));
	};

	fall = [this](float time_over)
	{
		std::vector<std::shared_ptr<Collision>> list;
		ColliderComponent::LineCheck(*mob->p, *mob->p + (mob->cam_facing - Vec3(mob->up)) * 2.0f, list);

		if (list.empty())
		{
			mob->move = Vec3();
		}

		checks.insert(std::make_pair(0.5f + time_over, fall));
	};
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
	if (mob == nullptr)
	{
		mob = entity->getComponent<MobComponent>();
	}

	if (mob != nullptr)
	{
		if (mob->p != nullptr)
		{
			mob->strafe = false;

			if (checks.empty())
			{
				checks.insert(std::make_pair(1.0f + dTime, idle));
				checks.insert(std::make_pair(dTime, fall));
			}
		}
	}

	std::multimap<float, std::function<void(float)>> nchecks;
	std::vector<std::pair<float, std::function<void(float)>>> checks_ready;
	for each (auto check in checks)
	{
		if (check.first - dTime <= 0.0f)
		{
			checks_ready.push_back(std::make_pair(check.first - dTime, check.second));
		}
		else
		{
			nchecks.insert(std::make_pair(check.first - dTime, check.second));
		}
	}
	checks = nchecks;

	for each (auto check in checks_ready)
	{
		check.second(check.first);
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