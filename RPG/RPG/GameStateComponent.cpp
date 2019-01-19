#include "GameStateComponent.h"

#include "World.h"

#include "Server.h"

#include "Client.h"
#include "ClientData.h"

#include "RenderSetup.h"

#include "Writing.h"

#include "TeamData.h"

#include "MobComponent.h"
#include "ChatComponent.h"

AutoSerialFactory<GameStateComponent, Component> GameStateComponent::_factory("GameStateComponent");

GameStateComponent::GameStateComponent(void) : Component(_factory.id)
{
}

GameStateComponent::GameStateComponent(instream& is, bool full) : Component(_factory.id)
{
}

GameStateComponent::~GameStateComponent(void)
{
}

void GameStateComponent::connect(NewEntity * pEntity, bool authority)
{
}

void GameStateComponent::disconnect(void)
{
	set_display(false);
}

void GameStateComponent::pre_frame(float dTime)
{
	set_display(true);
}

void GameStateComponent::post_frame(float dTime)
{
}

void GameStateComponent::tick(float dTime)
{
	if (entity->world->authority)
	{
		
	}
}

void GameStateComponent::writeLog(outstream& os, ClientData& client)
{
}

void GameStateComponent::readLog(instream& is)
{
}

void GameStateComponent::writeLog(outstream& os)
{
	os << 0;
}

void GameStateComponent::readLog(instream& is, ClientData& client)
{
	int dummy;
	is >> dummy;
	auto avatar = avatars.find(client.client_id);
	if (avatar == avatars.end())
	{
		avatars.insert(std::make_pair(client.client_id, createAvatar(client.client_id, 0, 0)));
	}
}

void GameStateComponent::interpolate(Component * pComponent, float fWeight)
{
	GameStateComponent * other = reinterpret_cast<GameStateComponent*>(pComponent);
}

void GameStateComponent::write_to(outstream& os, ClientData& client) const
{
}

void GameStateComponent::write_to(outstream& os) const
{
}

#include "CameraControlComponent.h"
#include "GraphicsComponent.h"
#include "PlayerInputComponent.h"
#include "MobComponent.h"
#include "AnimationControlComponent.h"
#include "InventoryComponent.h"
#include "PoseComponent.h"
#include "ProjectileComponent.h"
#include "WeaponComponent.h"
#include "ColliderComponent.h"
#include "AudioComponent.h"
#include "CameraShakeComponent.h"
#include "HitComponent.h"

#include "SimpleState.h"

MobComponent * GameStateComponent::createAvatar(uint32_t client_id, uint32_t team, uint32_t index)
{
	NewEntity * ent = new NewEntity();

	CameraControlComponent * cam = new CameraControlComponent();
	PositionComponent * p = new PositionComponent();
	GraphicsComponent * g = new GraphicsComponent(false);
	PlayerInputComponent * input = new PlayerInputComponent();
	MobComponent * mob = new MobComponent();
	AnimationControlComponent * acc = new AnimationControlComponent();
	InventoryComponent * inv = new InventoryComponent();
	PoseComponent * pose = new PoseComponent();

	ent->addComponent(cam);
	ent->addComponent(p);
	ent->addComponent(g);
	ent->addComponent(input);
	ent->addComponent(mob);
	ent->addComponent(acc);
	ent->addComponent(inv);
	ent->addComponent(pose);

	pose->anim = "data/assets/units/player/hoodlum.anim";
	g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/player/hoodlum.gmdl", Material("data/assets/units/player/hoodlum.tga"))));

	input->client_id = client_id;
	cam->client_id = client_id;
	inv->client_id = client_id;

	mob->p = p;

	entity->world->AddEntity(ent);

	if (team == 0)
	{
		NewEntity * ent = new NewEntity();

		PositionComponent * p = new PositionComponent();
		GraphicsComponent * g = new GraphicsComponent();
		WeaponComponent * w = new WeaponComponent();

		ent->addComponent(p);
		ent->addComponent(g);
		ent->addComponent(w);

		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/items/weapons/swords/claymore.gmdl", Material("data/assets/items/weapons/swords/claymore.tga"), 0)));
		g->decs.items.back()->local = Matrix4::Scale(0.5f) * Quaternion(M_PI_2, Vec3(0.0f, 1.0f, 0.0f));

		mob->weapon = w;
		w->mob_id = mob->entity->get_id();

		entity->world->AddEntity(ent);

		w->updateHitbox();
	}

	return mob;
}

void GameStateComponent::set_display(bool enable)
{
	if (!entity->world)
		return;
	Client * client = entity->world->client;
	if (client != nullptr)
	{
		if (enable)
		{
			if (!func)
			{
				func.reset(new std::function<void(RenderSetup&)>([this](RenderSetup& rs)
				{

				}));
				client->render2D.push_back(func);
			}
		}
		else
		{
			if (func)
			{
				client->render2D.erase(std::remove(client->render2D.begin(), client->render2D.end(), func), client->render2D.end());
				func.reset();
			}
		}
	}
}