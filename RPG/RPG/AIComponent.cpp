#include "AIComponent.h"

#include "NewEntity.h"
#include "World.h"
#include "MobComponent.h"
#include "ColliderComponent.h"
#include "AnimationControlComponent.h"

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

		auto nearby_mobs = entity->world->GetNearestComponents<MobComponent>(*mob->p, 10.0f);
		MobComponent * other = nullptr;
		for each (auto nearby in nearby_mobs)
		{
			if (nearby.second->temp_team != mob->temp_team)
			{
				std::vector<std::shared_ptr<Collision>> list;
				ColliderComponent::LineCheck(*mob->p, *nearby.second->p, list);
				if (list.empty())
				{
					other = nearby.second;
					break;
				}
			}
		}

		if (other != nullptr)
		{
			Vec3 dif = *other->p - *mob->p;
			dif.Normalize();
			mob->move = dif;
			mob->cam_facing = dif;

			checks.insert(std::make_pair((uni_dist(random) + uni_dist(random)) * 1.0f + time_over, chase));
		}
		else
		{
			checks.insert(std::make_pair((uni_dist(random) + uni_dist(random)) * 4.0f + time_over, idle));
		}
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

	chase = [this](float time_over)
	{
		std::uniform_real_distribution<float> uni_dist;

		auto nearby_mobs = entity->world->GetNearestComponents<MobComponent>(*mob->p + mob->cam_facing * 10.0f);
		MobComponent * other = nullptr;
		for each (auto nearby in nearby_mobs)
		{
			if (nearby.second->temp_team != mob->temp_team)
			{
				std::vector<std::shared_ptr<Collision>> list;
				ColliderComponent::LineCheck(*mob->p, *nearby.second->p, list);
				if (list.empty())
				{
					other = nearby.second;
					break;
				}
			}
		}

		if (other != nullptr)
		{
			Vec3 dif = *other->p - *mob->p;
			float l = dif.Len();
			dif *= Quaternion((uni_dist(random) - uni_dist(random)) * (0.05f + atan(l / 100.0f)), Vec3(0.0f, 0.0f, 1.0f));
			l = dif.Len();
			dif /= l;
			mob->move += dif;
			mob->move.Normalize();
			mob->cam_facing = mob->move;

			if (l < 1.5f)
			{
				mob->move = Vec3();
				checks.insert(std::make_pair(0.5f + time_over, attack));
				auto acc = entity->getComponent<AnimationControlComponent>();
				if (acc)
				{
					acc->set_state(3);
				}
			}
			else
			{
				checks.insert(std::make_pair((uni_dist(random) + uni_dist(random)) * 0.25f + 0.25f + time_over, chase));
			}
		}
		else
		{
			checks.insert(std::make_pair((uni_dist(random) + uni_dist(random)) * 0.25f + 0.25f + time_over, search));
		}
	};


	search = [this](float time_over)
	{
		std::uniform_real_distribution<float> uni_dist;

		auto nearby_mobs = entity->world->GetNearestComponents<MobComponent>(*mob->p + mob->cam_facing * 10.0f);
		MobComponent * other = nullptr;
		for each (auto nearby in nearby_mobs)
		{
			if (nearby.second->temp_team != mob->temp_team)
			{
				std::vector<std::shared_ptr<Collision>> list;
				ColliderComponent::LineCheck(*mob->p, *nearby.second->p, list);
				if (list.empty())
				{
					other = nearby.second;
					break;
				}
			}
		}

		if (other != nullptr)
		{
			Vec3 dif = *other->p - *mob->p;
			dif.Normalize();
			mob->move = dif;
			mob->cam_facing = dif;

			checks.insert(std::make_pair((uni_dist(random) + uni_dist(random)) * 0.25f + 0.25f + time_over, chase));
		}
		else
		{
			if (uni_dist(random) > 0.95f)
			{
				checks.insert(std::make_pair((uni_dist(random) + uni_dist(random)) * 0.25f + 0.25f + time_over, idle));
			}
			else
			{
				checks.insert(std::make_pair((uni_dist(random) + uni_dist(random)) * 0.25f + 0.25f + time_over, search));
			}
		}
	};

	attack = [this](float time_over)
	{
		std::uniform_real_distribution<float> uni_dist;

		auto nearby_mobs = entity->world->GetNearestComponents<MobComponent>(*mob->p + mob->cam_facing * 0.75f, 0.75f);
		MobComponent * other = nullptr;
		for each (auto nearby in nearby_mobs)
		{
			if (nearby.second->temp_team != mob->temp_team)
			{
				std::vector<std::shared_ptr<Collision>> list;
				ColliderComponent::LineCheck(*mob->p, *nearby.second->p, list);
				if (list.empty())
				{
					other = nearby.second;
					break;
				}
			}
		}

		if (other != nullptr)
		{
			other->hit = true;
			other->health.current -= 5.0f;
		}

		checks.insert(std::make_pair(0.5f + time_over, chase));
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