#include "World.h"
#include "Client.h"
#include "Server.h"

#include "GameLoop.h"

#include "NewEntity.h"

#include "PositionComponent.h"
#include "GraphicsComponent.h"
#include "MobComponent.h"
#include "PlayerInputComponent.h"
#include "CameraControlComponent.h"
#include "AnimationControlComponent.h"
#include "ColliderComponent.h"
#include "InventoryComponent.h"
#include "HitComponent.h"
#include "LineComponent.h"
#include "LightComponent.h"
#include "AIComponent.h"
#include "PoseComponent.h"
#include "OrbitComponent.h"
#include "WeaponComponent.h"
#include "SpawnComponent.h"
#include "ProjectileComponent.h"
#include "GameStateComponent.h"
#include "TriggerComponent.h"
#include "ChatComponent.h"

#include "ClientData.h"

#include "Matrix3.h"

#include <regex>

class MyServer :
	public Server
{
public:
	MyServer(World * pWorld) : Server(pWorld) {}

	void onServerActivated(void)
	{
		// create game state
		auto previous_game_states = world->GetComponents<GameStateComponent>();
		GameStateComponent * game_state;
		if (previous_game_states.empty())
		{
			NewEntity * ent = new NewEntity();

			game_state = new GameStateComponent();

			ent->addComponent(game_state);

			world->AddEntity(ent);
		}
		else
		{
			game_state = previous_game_states.front();
		}

		// create chat
		{
			NewEntity * ent = new NewEntity();

			ChatComponent * chat = new ChatComponent();

			ent->addComponent(chat);

			world->AddEntity(ent);
		}

		// create level
		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent();
			ColliderComponent * c = new ColliderComponent();
			GraphicsComponent * g = new GraphicsComponent(false);

			ent->addComponent(p);
			ent->addComponent(c);
			ent->addComponent(g);

			std::string ao = "data/assets/escape-ao.tga";

			//Resource::load(ao, { "!sRGB" });

			MaterialList materials;
			materials.materials.push_back(Material("data/assets/terrain/textures/concrete.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/asphalt.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/plank.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/nground.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/RockPlate.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/brick2.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/RockPlate.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/RockPlate.tga"));

			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/escape.gmdl", materials, 0)));
			g->decs.items.front()->local *= 10.0f;
			g->decs.items.front()->local.mtrx[3][3] = 1.0f;
			g->decs.items.front()->final = g->decs.items.front()->local;

			world->AddEntity(ent);
		}

		auto createProgressZone = [game_state, this](const GlobalPosition& aabb_min, const GlobalPosition& aabb_max, float progress)
		{
			NewEntity * ent = new NewEntity();

			TriggerComponent * trigger = new TriggerComponent();

			ent->addComponent(trigger);

			trigger->aabb_min = aabb_min;
			trigger->aabb_max = aabb_max;

			trigger->delay = 1.0f;
			trigger->countdown = progress;

			trigger->func = [game_state, progress](MobComponent * mob)
			{
				game_state->setProgress(mob, progress);
			};

			world->AddEntity(ent);
		};

		// create spawner
		{
			NewEntity * ent = new NewEntity();

			SpawnComponent * spawn = new SpawnComponent();

			ent->addComponent(spawn);

			spawn->aabb_min = Vec3(21.0f, 20.0f, 21.0f);
			spawn->aabb_max = Vec3(41.0f, 40.0f, 30.0f);

			world->AddEntity(ent);

			for (size_t i = 0; i < 20; i++)
			{
				spawn->spawn();
			}
		}

		// create spawner
		{
			NewEntity * ent = new NewEntity();

			SpawnComponent * spawn = new SpawnComponent();

			ent->addComponent(spawn);

			spawn->aabb_min = Vec3(21.0f, -10.0f, 19.0f);
			spawn->aabb_max = Vec3(41.0f, 20.0f, 25.0f);

			world->AddEntity(ent);

			for (size_t i = 0; i < 20; i++)
			{
				spawn->spawn();
			}
		}

		SpawnComponent * start_roof_spawn;
		// create spawner
		{
			NewEntity * ent = new NewEntity();

			SpawnComponent * spawn = new SpawnComponent();

			ent->addComponent(spawn);

			spawn->aabb_min = Vec3(-20.0f, -9.0f, 32.0f);
			spawn->aabb_max = Vec3(-10.0f, 11.0f, 34.0f);

			world->AddEntity(ent);

			for (size_t i = 0; i < 5; i++)
			{
				spawn->spawn();
			}

			start_roof_spawn = spawn;
		}

		SpawnComponent * road_chase_riverside_spawn;
		// create spawner
		{
			NewEntity * ent = new NewEntity();

			SpawnComponent * spawn = new SpawnComponent();

			ent->addComponent(spawn);

			spawn->aabb_min = Vec3(-45.0f, 60.0f, 0.0f);
			spawn->aabb_max = Vec3(-40.0f, 69.0f, 2.0f);

			world->AddEntity(ent);

			road_chase_riverside_spawn = spawn;
		}

		SpawnComponent * road_hill_front_spawn;
		// create spawner
		{
			NewEntity * ent = new NewEntity();

			SpawnComponent * spawn = new SpawnComponent();

			ent->addComponent(spawn);

			spawn->aabb_min = Vec3(70.0f, -100.0f, 20.0f);
			spawn->aabb_max = Vec3(80.0f, -110.0f, 22.0f);

			world->AddEntity(ent);

			road_hill_front_spawn = spawn;
		}

		// create trigger volume
		{
			NewEntity * ent = new NewEntity();

			TriggerComponent * trigger = new TriggerComponent();

			ent->addComponent(trigger);

			trigger->aabb_min = Vec3(-10.0f, -11.0f, 22.0f);
			trigger->aabb_max = Vec3(12.0f, 13.0f, 30.0f);

			trigger->delay = 5.0f;

			trigger->func = [game_state, start_roof_spawn](MobComponent * mob)
			{
				for (size_t i = 0; i < 5; i++)
				{
					start_roof_spawn->spawn(Vec3(1.0f, 0.0f, 0.0f));
				}
			};

			world->AddEntity(ent);
		}

		// create trigger volume
		{
			NewEntity * ent = new NewEntity();

			TriggerComponent * trigger = new TriggerComponent();

			ent->addComponent(trigger);

			trigger->aabb_min = Vec3(-110.0f, -80.0f, 0.0f);
			trigger->aabb_max = Vec3(-20.0f, 0.0f, 5.0f);

			trigger->delay = 1.0f;

			trigger->func = [game_state, road_chase_riverside_spawn](MobComponent * mob)
			{
				for (size_t i = 0; i < 2; i++)
				{
					road_chase_riverside_spawn->spawn(Vec3(-1.0f, 0.0f, 0.0f));
				}
			};

			world->AddEntity(ent);
		}

		// create trigger volume
		{
			NewEntity * ent = new NewEntity();

			TriggerComponent * trigger = new TriggerComponent();

			ent->addComponent(trigger);

			trigger->aabb_min = Vec3(70.0f, -20.0f, 0.0f);
			trigger->aabb_max = Vec3(90.0f, 45.0f, 2.0f);

			trigger->delay = 1.0f;

			trigger->func = [game_state, road_hill_front_spawn](MobComponent * mob)
			{
				for (size_t i = 0; i < 5; i++)
				{
					road_hill_front_spawn->spawn(Vec3(0.0f, 1.0f, 0.0f));
				}
			};

			world->AddEntity(ent);
		}

		createProgressZone(Vec3(-10.0f, -11.0f, 22.0f), Vec3(12.0f, 13.0f, 30.0f), 0.05f);
		createProgressZone(Vec3(21.0f, -10.0f, 19.0f), Vec3(41.0f, 40.0f, 30.0f), 0.1f);

		createProgressZone(Vec3(48.0f, -21.0f, 20.0f), Vec3(70.0f, 49.0f, 25.0f), 0.15f);
		createProgressZone(Vec3(48.0f, -21.0f, 14.0f), Vec3(70.0f, 49.0f, 19.0f), 0.2f);
		createProgressZone(Vec3(48.0f, -21.0f, 8.0f), Vec3(70.0f, 49.0f, 13.0f), 0.25f);
		createProgressZone(Vec3(48.0f, -21.0f, 2.0f), Vec3(70.0f, 49.0f, 7.0f), 0.3f);

		createProgressZone(Vec3(70.0f, -20.0f, 0.0f), Vec3(90.0f, 45.0f, 2.0f), 0.35f);
		createProgressZone(Vec3(60.0f, -80.0f, 0.0f), Vec3(90.0f, -20.0f, 2.0f), 0.4f);
		createProgressZone(Vec3(0.0f, -80.0f, 0.0f), Vec3(60.0f, -60.0f, 2.0f), 0.45f);
		createProgressZone(Vec3(-20.0f, -80.0f, 0.0f), Vec3(0.0f, -20.0f, 2.0f), 0.5f);
		createProgressZone(Vec3(0.0f, -30.0f, 0.0f), Vec3(40.0f, -10.0f, 2.0f), 0.55f);

		createProgressZone(Vec3(41.0f, -10.0f, 0.0f), Vec3(48.0f, 40.0f, 2.0f), 0.6f);
		createProgressZone(Vec3(-10.0f, 10.0f, 0.0f), Vec3(20.0f, 40.0f, 2.0f), 0.65f);
		createProgressZone(Vec3(-50.0f, 40.0f, 0.0f), Vec3(10.0f, 60.0f, 2.0f), 0.7f);

		createProgressZone(Vec3(10.0f, 47.0f, 0.0f), Vec3(40.0f, 60.0f, 2.0f), 0.75f);
		createProgressZone(Vec3(-50.0f, 60.0f, 0.0f), Vec3(-10.0f, 70.0f, 2.0f), 0.8f);

		createProgressZone(Vec3(-100.0f, 0.0f, 0.0f), Vec3(-30.0f, 40.0f, 2.0f), 0.85f);
		createProgressZone(Vec3(-100.0f, -80.0f, 0.0f), Vec3(-30.0f, 0.0f, 2.0f), 0.9f);
		createProgressZone(Vec3(-100.0f, -160.0f, 0.0f), Vec3(-30.0f, -80.0f, 2.0f), 0.95f);
		createProgressZone(Vec3(-100.0f, -160.0f, 0.0f), Vec3(-70.0f, -140.0f, 2.0f), 1.0f);
	}

	void onClientConnect(ClientData& data)
	{
		auto chats = world->GetComponents<ChatComponent>();
		if (chats.size())
		{
			ChatMessage message;
			message.message = "Client id " + std::to_string(data.client_id) + " connected.\n";
			message.timeout = 10.0f;
			chats.front()->messages.push_back(message);
		}
	}
};

#include "SteamWrapper.h"

//int main(int argc, char* argv[])
INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
	std::string address;
	uint16_t port = 7777;
	uint64_t lobby_id = 0;
	char option = 'h';

	std::cout << "starting game..." << std::endl;

	std::cmatch match_connect;
	std::regex reg_connect("\\+connect (.*):(.*)");
	if (std::regex_match(lpCmdLine, match_connect, reg_connect))
	{
		option = 'c';
		address = match_connect[1];
		port = std::atoi(match_connect[2].str().c_str());
	}

	std::cmatch match_connect_lobby;
	std::regex reg_connect_lobby("\\+connect_lobby (.*)");
	std::regex_match(lpCmdLine, match_connect_lobby, reg_connect_lobby);

	//std::shared_ptr<ISteamWrapper> steam(ISteamWrapper::make());
	
	World * world = new World();
	Server * server = new MyServer(world);
	Client * client = new Client(world);

	if (option=='p')
	{
		server->onServerActivated();
		server->onClientConnect(*client->clientData);
	}
	if (option=='c')
	{
		client->connect(address, port);
	}
	if (option == 'h')
	{
		server->open(port);
		server->onServerActivated();
		server->onClientConnect(*client->clientData);
	}
	if (option=='s')
	{
		server->open(port);
	}

	world->client = client;
	world->server = server;
	if (server != nullptr)
	{
		server->client = client;
		if (client != nullptr)
			client->server = server;
		GameLoop gl(world, server, client);
		gl.init();
		if (client != nullptr)
		{
			while (client->IsAlive())
				gl.tick();
		}
		else
		{
			while (true)
				gl.tick();
		}
	}
}