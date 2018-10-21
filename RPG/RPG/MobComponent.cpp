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

MobComponent::MobComponent(void) : Serializable(_factory.id), health(10), stamina(10), mana(10), scene(64, 64)
{
	facing = Vec2(0.0f, 1.0f);

	memset(scene.tiles, 0, sizeof(Tile) * scene.w * scene.h);

	Tile * iterator = scene.tiles;
	for (size_t y = 0; y < scene.h; ++y)
	{
		for (size_t x = 0; x < scene.w; ++x)
		{
			if (y == 0 || y == scene.h - 1 || x == 0 || x == scene.w - 1)
			{
				iterator->solid = 1;
			}
			++iterator;
		}
	}

	scene.getTile(4, 4)->solid = 1;
	scene.getTile(4, 2)->solid = 1;
	scene.getTile(5, 2)->solid = 1;

	mob.x = 6.0f;
	mob.y = 4.0f;
	mob.r = 0.25f;
}

MobComponent::MobComponent(instream& is, bool full) : Serializable(_factory.id), scene(64, 64)
{
	is >> facing;

	memset(scene.tiles, 0, sizeof(Tile) * scene.w * scene.h);

	Tile * iterator = scene.tiles;
	for (size_t y = 0; y < scene.h; ++y)
	{
		for (size_t x = 0; x < scene.w; ++x)
		{
			if (y == 0 || y == scene.h - 1 || x == 0 || x == scene.w - 1)
			{
				iterator->solid = 1;
			}
			++iterator;
		}
	}

	scene.getTile(4, 4)->solid = 1;
	scene.getTile(4, 2)->solid = 1;
	scene.getTile(5, 2)->solid = 1;

	mob.x = 6.0f;
	mob.y = 4.0f;
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
	if (move != Vec2() && entity->world->authority)
	{
		facing = move.Normalized();

		Vec2 target = move;
		target *= 4.0f * dTime;

		mob.move(target, scene);
	}

	auto pc = entity->getComponent<PositionComponent>();
	if (pc != nullptr)
	{
		pc->p = Vec3(mob.p);
	}
}

void MobComponent::writeLog(outstream& os, ClientData& client)
{
	os << facing << move << mob.p;
	os << health << stamina << mana;
}

void MobComponent::readLog(instream& is)
{
	is >> facing >> move >> mob.p;
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
		facing = bu_sphere(other->facing, facing, Vec3(0.0f, 0.0f, 1.0f), fWeight);
		health = other->health;
		stamina = other->stamina;
		mana = other->mana;
		mob.p = bu_blend(mob.p, other->mob.p, fWeight);
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