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

GameStateComponent::GameStateComponent(instream& is) : Component(_factory.id)
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

void GameStateComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
}

void GameStateComponent::readLog(instream& is)
{
}

void GameStateComponent::writeLog(outstream& os)
{
	os << 0;
}

void GameStateComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
	int dummy;
	is >> dummy;
	auto avatar = avatars.find(client->client_id);
	if (avatar == avatars.end())
	{
		avatars.insert(std::make_pair(client->client_id, createAvatar(client, 0, 0)));
	}
}

void GameStateComponent::interpolate(Component * pComponent, float fWeight)
{
	GameStateComponent * other = reinterpret_cast<GameStateComponent*>(pComponent);
}

void GameStateComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
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

MobComponent * GameStateComponent::createAvatar(const std::shared_ptr<ClientData>& client, uint32_t team, uint32_t index)
{
	NewEntity * ent = new NewEntity();

	CameraControlComponent * cam = new CameraControlComponent();
	PositionComponent * p = new PositionComponent(Vec3(0.0f, 0.0f, 250.0f));
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

	input->clientData = client;
	cam->client_id = client->client_id;
	inv->clientData = client;

	mob->p = p;
	mob->respawn = p->p;

	entity->world->AddEntity(ent);

	mob->abilities.add(std::make_shared<uint32_t>(1));
	mob->abilities.add(std::make_shared<uint32_t>(0));
	mob->abilities.add(std::make_shared<uint32_t>(1));
	mob->abilities.add(std::make_shared<uint32_t>(0));
	mob->abilities.add(std::make_shared<uint32_t>(1));
	mob->abilities.add(std::make_shared<uint32_t>(0));
	mob->abilities.add(std::make_shared<uint32_t>(1));
	mob->abilities.add(std::make_shared<uint32_t>(0));

	if (team == 0)
	{
		NewEntity * ent = new NewEntity();

		PositionComponent * p = new PositionComponent();
		GraphicsComponent * g = new GraphicsComponent();
		WeaponComponent * w = new WeaponComponent();

		ent->addComponent(p);
		ent->addComponent(g);
		ent->addComponent(w);

		w->mob_id = mob->entity->get_id();
		mob->weapon = w;

		entity->world->AddEntity(ent);

		w->item_index = -1;
		w->update();
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
				func = std::make_shared<std::function<void(RenderSetup&)>>([this](RenderSetup& rs)
				{

				});
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