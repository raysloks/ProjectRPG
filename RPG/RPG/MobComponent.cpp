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

MobComponent::MobComponent(void) : Serializable(_factory.id), health(40), mana(25)
{
	facing = Vec3(0.0f, 1.0f, 0.0f);
	speed = 3.0f;
	r = 0.5f;
}

MobComponent::MobComponent(instream& is, bool full) : Serializable(_factory.id)
{
	is >> facing;
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

#include "NavigationMesh.h"

void MobComponent::tick(float dTime)
{
	if (!entity->world->authority)
		return;

	std::shared_ptr<NavigationMesh> navmesh;
	auto mesh = Resource::get<Mesh>("data/assets/plane8x8.gmdl");
	if (mesh)
	{
		navmesh.reset(new NavigationMesh(*mesh));
	}

	auto target_ent = entity->world->GetEntity(target);
	if (target_ent)
	{
		auto target_p = target_ent->getComponent<PositionComponent>();
		if (target_p)
		{
			target_location = target_p->p;
		}
	}

	v = Vec3();
	auto p = entity->getComponent<PositionComponent>();
	if (p)
	{
		if (navmesh && path.empty())
		{
			path = navmesh->GetPath(Vec3(p->p), Vec3(target_location));
		}

		if (!path.empty())
		{
			Vec3 dif = Vec3(path.back()) - p->p;
			float l = dif.Len();
			if (l > 0.1f)
			{
				Vec3 dir = dif / l;
				facing = dir;
				v = dir * speed;
				p->p += v * dTime;
				p->update();
			}
			else
			{
				path.pop_back();
			}
		}
	}
}

void MobComponent::writeLog(outstream& os, ClientData& client)
{
	os << facing << v << health << mana << r;
}

void MobComponent::readLog(instream& is)
{
	is >> facing >> v >> health >> mana >> r;
}

void MobComponent::writeLog(outstream& os)
{
}

void MobComponent::readLog(instream& is, ClientData& client)
{
}

void MobComponent::interpolate(Component * pComponent, float fWeight)
{
	auto mob = reinterpret_cast<MobComponent*>(pComponent);
	if (mob != nullptr)
	{
		Vec3 up(0.0f, 0.0f, 1.0f);
		facing = bu_sphere(mob->facing, facing, up, fWeight);
		v = mob->v;
		health = mob->health;
		mana = mob->mana;
		r = mob->r;
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
	if (damage == 0)
		return;

	if (health.current > 0)
	{
		health.current -= damage;
		if (health.current <= 0)
		{
			health.current = 0;
		}
	}
	else
	{
		health.current -= damage;
	}
}

void MobComponent::do_heal(size_t heal, EntityID source)
{
	health.current += heal;
	if (health.current > health.max)
		health.current = health.max;
}