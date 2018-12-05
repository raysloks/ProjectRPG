#include "MobComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "PositionComponent.h"
#include "CameraControlComponent.h"
#include "ColliderComponent.h"
#include "GraphicsComponent.h"
#include "ProjectileComponent.h"
#include "WeaponComponent.h"
#include "PoseComponent.h"
#include "AnimationControlComponent.h"
#include "AIComponent.h"
#include "GameStateComponent.h"

#include "GlobalPosition.h"

#include "ClientData.h"

#include "Client.h"

#include "Input.h"

#include "BlendUtility.h"

#include "SimpleState.h"

const AutoSerialFactory<MobComponent> MobComponent::_factory("MobComponent");

MobComponent::MobComponent(void) : Serializable(_factory.id), health(10), stamina(10), mana(10), scene(8, 8, 8)
{
	facing = Vec2(0.0f, 1.0f);

	for (size_t z = 0; z < scene.d * CHUNK_SIZE; ++z)
	{
		for (size_t y = 0; y < scene.h * CHUNK_SIZE; ++y)
		{
			for (size_t x = 0; x < scene.w * CHUNK_SIZE; ++x)
			{
				if (z + 8 <= x / 4 || z == 0)
				{
					scene.setTile(x, y, z, { 1 });
				}
			}
		}
	}

	mob.x = 6.0f;
	mob.y = 4.0f;
	mob.z = 2.0f;
	mob.r = 0.25f;
}

MobComponent::MobComponent(instream& is, bool full) : Serializable(_factory.id), scene(8, 8, 8)
{
	is >> facing;

	for (size_t z = 0; z < scene.d * CHUNK_SIZE; ++z)
	{
		for (size_t y = 0; y < scene.h * CHUNK_SIZE; ++y)
		{
			for (size_t x = 0; x < scene.w * CHUNK_SIZE; ++x)
			{
				if (z + 8 <= x / 4 || z == 0)
				{
					scene.setTile(x, y, z, { 1 });
				}
			}
		}
	}

	mob.x = 6.0f;
	mob.y = 4.0f;
	mob.z = 2.0f;
	mob.r = 0.25f;
}

MobComponent::~MobComponent(void)
{
}

void MobComponent::connect(NewEntity * pEntity, bool authority)
{
}

void MobComponent::disconnect(void)
{
}

#include "Profiler.h"

#include "LineComponent.h"
#include "AudioComponent.h"

void MobComponent::tick(float dTime)
{
	for (auto& timer : timers)
	{
		timer.second -= dTime;
	}

	auto acc = entity->getComponent<AnimationControlComponent>();
	if (entity->world->authority && acc)
	{
		if (move != Vec2())
		{
			Vec2 target = move;
			target *= 4.0f * dTime;

			if (acc->has_state("attack"))
				target *= 0.5f;

			mob.move(target, scene);
		}

		if (!acc->has_state("attack"))
		{
			if (timers["attack"] > 0.0f)
			{
				auto acc = entity->getComponent<AnimationControlComponent>();
				if (acc)
				{
					acc->set_state(new SimpleState("attack", 3.0f));
					timers["attack"] = 0.0f;
				}
			}
			if (timers["jump"] > 0.0f && mob.land)
			{
				mob.v += Vec3(0.0f, 0.0f, 5.0f);
				timers["jump"] = 0.0f;
			}
		}

		Vec3 g(0.0f, 0.0f, -9.8f);

		mob.move_velocity((mob.v + g * dTime) * dTime, scene);

		mob.v += g * dTime;
	}

	auto pc = entity->getComponent<PositionComponent>();
	if (pc != nullptr)
	{
		pc->p = mob.p + Vec3(0.0f, 0.0f, -mob.r);
	}
}

void MobComponent::writeLog(outstream& os, ClientData& client)
{
	os << facing << move << mob.p << mob.land;
	os << health << stamina << mana;
}

void MobComponent::readLog(instream& is)
{
	is >> facing >> move >> mob.p >> mob.land;
	is >> health >> stamina >> mana;
}

void MobComponent::writeLog(outstream& os)
{
}

void MobComponent::readLog(instream& is, ClientData& client)
{
}

void MobComponent::interpolate(Component * pComponent, float fWeight)
{
	auto other = reinterpret_cast<MobComponent*>(pComponent);
	if (other != nullptr)
	{
		facing.x = bu_angle(other->facing.x, facing.x, fWeight);
		facing.y = bu_blend(other->facing.y, facing.y, fWeight);
		health = other->health;
		stamina = other->stamina;
		mana = other->mana;
		mob.p = bu_blend(mob.p, other->mob.p, fWeight);
		mob.land = other->mob.land;
		move = bu_blend(move, other->move, fWeight);
	}
}

void MobComponent::write_to(outstream& os, ClientData& client) const
{
	os << facing;
}

void MobComponent::write_to(outstream& os) const
{
}

void MobComponent::do_damage(size_t damage, EntityID source)
{
}

void MobComponent::do_heal(size_t heal, EntityID source)
{
}