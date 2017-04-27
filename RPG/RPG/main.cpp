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

#include "ClientData.h"

#include "Matrix3.h"

#include <regex>

class MyServer :
	public Server
{
public:
	MyServer(World * pWorld) : Server(pWorld) {}

	void onClientConnect(ClientData& data)
	{
		if (world->units.empty())
		{
			// create game state
			{
				NewEntity * ent = new NewEntity();

				GameStateComponent * game_state = new GameStateComponent();

				ent->addComponent(game_state);

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

				g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/escape.gmdl", materials, 0)));
				g->decs.items.front()->local *= 10.0f;
				g->decs.items.front()->local.mtrx[3][3] = 1.0f;
				g->decs.items.front()->final = g->decs.items.front()->local;

				world->AddEntity(ent);
			}

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

			// create trigger volume
			{
				NewEntity * ent = new NewEntity();

				TriggerComponent * trigger = new TriggerComponent();

				ent->addComponent(trigger);

				trigger->aabb_min = Vec3(-10.0f, -11.0f, 22.0f);
				trigger->aabb_max = Vec3(12.0f, 13.0f, 30.0f);

				trigger->delay = 1.0f;

				trigger->func = [start_roof_spawn](MobComponent * mob)
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

				trigger->aabb_min = Vec3(21.0f, -10.0f, 19.0f);
				trigger->aabb_max = Vec3(41.0f, 40.0f, 30.0f);

				trigger->delay = 1.0f;

				trigger->func = [](MobComponent * mob)
				{
				};

				world->AddEntity(ent);
			}
		}

		// create player entity
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
			LineComponent * line = new LineComponent();

			ent->addComponent(cam);
			ent->addComponent(p);
			ent->addComponent(g);
			ent->addComponent(input);
			ent->addComponent(mob);
			ent->addComponent(acc);
			ent->addComponent(inv);
			ent->addComponent(pose);
			ent->addComponent(line);

			p->p = Vec3(-15.0f, -5.0f, 23.0f);

			/*for (int i = 1; i < 23; ++i)
			{
				LightComponent * light = new LightComponent();
				light->bone_id = i;
				ent->addComponent(light);
			}*/

			/*g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cylinder32.gmdl", Material("data/assets/empty.tga"), 0)));
			g->decs.items.front()->local *= 0.5f;
			g->decs.items.front()->local.mtrx[1][1] = 0.25f;
			g->decs.items.front()->local.data[15] = 1.0f;
			g->decs.items.front()->final = g->decs.items.front()->local;*/

			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/player/KnightGuy.gmdl", Material("data/assets/units/player/KnightGuy.tga"), 0)));
			//g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/eyes/left.gmdl", Material("data/assets/decorators/eyes/basic.tga"))));
			//g->decs.items.back()->priority = 1;
			//g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/eyes/right.gmdl", Material("data/assets/decorators/eyes/basic.tga"))));
			//g->decs.items.back()->priority = 2;
			//g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/mouth/mouth.gmdl", Material("data/assets/decorators/mouth/neutral.tga"))));
			//g->decs.items.back()->priority = 3;

			mob->attack = [mob]()
			{
				auto spawn_bullet = [mob](const Vec3& muzzle_velocity)
				{
					NewEntity * ent = new NewEntity();

					PositionComponent * pos = new PositionComponent();
					ProjectileComponent * projectile = new ProjectileComponent();
					GraphicsComponent * g = new GraphicsComponent(false);

					ent->addComponent(pos);
					ent->addComponent(projectile);
					ent->addComponent(g);

					pos->p = *mob->p + mob->up * 0.45f;

					projectile->v = muzzle_velocity + mob->v;
					projectile->drag = 0.01f;

					g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cube.gmdl", Material("data/assets/empty.tga"), 0)));
					g->decs.items.front()->local *= 0.0127f * 0.5f;
					g->decs.items.front()->local.data[15] = 1.0f;
					g->decs.items.front()->local *= mob->cam_rot;

					mob->entity->world->AddEntity(ent);
				};

				spawn_bullet(mob->cam_facing * 470.0f);

				mob->recoil += 0.3f;

				mob->input.erase("attack");
			};

			input->client_id = data.client_id;
			cam->client_id = data.client_id;
			inv->client_id = data.client_id;

			world->AddEntity(ent);

			{
				NewEntity * ent = new NewEntity();

				PositionComponent * p = new PositionComponent();
				GraphicsComponent * g = new GraphicsComponent();
				WeaponComponent * w = new WeaponComponent();

				ent->addComponent(p);
				ent->addComponent(g);
				ent->addComponent(w);

				g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/pistol.gmdl", Material("data/assets/pistol.tga"), 0)));

				mob->weapon = w;

				world->AddEntity(ent);
			}
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

	std::shared_ptr<ISteamWrapper> steam(ISteamWrapper::make());
	
	World * world = new World();
	Server * server = new MyServer(world);
	Client * client = new Client(world);

	if (option=='p')
	{
		server->onClientConnect(*client->clientData);
	}
	if (option=='c')
	{
		client->connect(address, port);
	}
	if (option == 'h')
	{
		server->open(port);
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