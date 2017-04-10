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

#include "ClientData.h"

#include "Matrix3.h"

class MyServer :
	public Server
{
public:
	MyServer(World * pWorld, unsigned short port) : Server(pWorld, port) {}
	MyServer(World * pWorld) : Server(pWorld) {}

	void onClientConnect(ClientData& data)
	{
		if (world->units.size() == 0)
		{
			/*{
				NewEntity * ent = new NewEntity();

				PositionComponent * p = new PositionComponent();
				GraphicsComponent * g = new GraphicsComponent();
				PlayerInputComponent * input = new PlayerInputComponent();
				MobComponent * mob = new MobComponent();
				AnimationControlComponent * acc = new AnimationControlComponent();
				InventoryComponent * inv = new InventoryComponent();

				ent->addComponent(p);
				ent->addComponent(g);
				ent->addComponent(input);
				ent->addComponent(mob);
				ent->addComponent(acc);
				ent->addComponent(inv);

				g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/player/KnightGuy.gmdl", "data/assets/empty.tga")));

				world->AddEntity(ent);
			}*/
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
			/*for (int x = -40; x < 40; ++x)
			{
				for (int y = -40; y < 40; ++y)
				{
					NewEntity * ent = new NewEntity();

					PositionComponent * p = new PositionComponent();
					p->p += Vec3(2.5f * x, 2.5f * y, -2.0f);
					ColliderComponent * c = new ColliderComponent();
					GraphicsComponent * g = new GraphicsComponent(false);

					ent->addComponent(p);
					ent->addComponent(c);
					ent->addComponent(g);

					std::vector<std::string> tex;
					tex.push_back("data/assets/terrain/textures/grass.tga");

					g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cube.gmdl", tex, 0)));
					g->decs.items.front()->local *= 1.0f;
					g->decs.items.front()->local.mtrx[3][3] = 1.0f;
					g->decs.items.front()->final = g->decs.items.front()->local;

					world->AddEntity(ent);
				}
			}*/
			/*for (int y = 0; y < 20; ++y)
			{
				for (int i = 0; i < 20; ++i)
				{
					NewEntity * ent = new NewEntity();

					PositionComponent * p = new PositionComponent();
					p->p += Vec3(0.2f * i, 0.2f * y + 0.05f * i, 0.15f * i + 0.05f * y + 20.0f);
					ColliderComponent * c = new ColliderComponent();
					GraphicsComponent * g = new GraphicsComponent(false);

					ent->addComponent(p);
					ent->addComponent(c);
					ent->addComponent(g);

					g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cube.gmdl", Material("data/assets/terrain/textures/RockPlate.tga"), 0)));
					g->decs.items.front()->local *= 0.1f;
					g->decs.items.front()->local.mtrx[3][3] = 1.0f;
					g->decs.items.front()->final = g->decs.items.front()->local;

					world->AddEntity(ent);
				}
			}*/
			/*{
				NewEntity * ent = new NewEntity();

				PositionComponent * p = new PositionComponent();
				p->p += Vec3(10.0f, 8.0f, -11.5f);
				ColliderComponent * c = new ColliderComponent();
				GraphicsComponent * g = new GraphicsComponent(false);

				ent->addComponent(p);
				ent->addComponent(c);
				ent->addComponent(g);

				std::vector<std::string> tex;
				tex.push_back("data/assets/terrain/textures/RockPlate.tga");

				g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/triangle_ds_test.gmdl", tex, 0)));
				g->decs.items.front()->final = g->decs.items.front()->local;

				world->AddEntity(ent);
			}
			{
				NewEntity * ent = new NewEntity();

				PositionComponent * p = new PositionComponent();
				p->p += Vec3(15.0f, 0.0f, -11.5f);
				ColliderComponent * c = new ColliderComponent();
				GraphicsComponent * g = new GraphicsComponent(false);

				ent->addComponent(p);
				ent->addComponent(c);
				ent->addComponent(g);

				std::vector<std::string> tex;
				tex.push_back("data/assets/terrain/textures/RockPlate.tga");

				g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/octagon_ds_test.gmdl", tex, 0)));
				g->decs.items.front()->final = g->decs.items.front()->local;

				world->AddEntity(ent);
			}*/
			/*for (int i = 0; i < 10; ++i)
			{
				NewEntity * ent = new NewEntity();

				PositionComponent * p = new PositionComponent();
				p->p += Vec3(5.0f, 0.0f, -1.0f + 2.0f * i);
				ColliderComponent * c = new ColliderComponent();
				GraphicsComponent * g = new GraphicsComponent(false);

				ent->addComponent(p);
				ent->addComponent(c);
				ent->addComponent(g);

				g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cube_bevel.gmdl", Material("data/assets/terrain/textures/RockPlate.tga"), 0)));
				g->decs.items.front()->final = g->decs.items.front()->local;

				world->AddEntity(ent);
			}*/
			/*{
				NewEntity * ent = new NewEntity();

				PositionComponent * p = new PositionComponent();
				p->p += Vec3(0.0f, 0.0f, 200.0f);
				ColliderComponent * c = new ColliderComponent();
				GraphicsComponent * g = new GraphicsComponent(false);
				OrbitComponent * orbit = new OrbitComponent();

				ent->addComponent(p);
				ent->addComponent(c);
				ent->addComponent(g);
				ent->addComponent(orbit);

				g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/props/rocks/rock.gmdl", Material("data/assets/props/rocks/rock.tga"), 0)));
				g->decs.items.front()->local *= 10.0f;
				g->decs.items.front()->local.data[15] = 1.0f;
				g->decs.items.front()->final = g->decs.items.front()->local;

				orbit->center = Vec3(0.0f, 0.0f, 0.0f);
				orbit->offset = p->p;
				orbit->angle = orbit->offset.Normalized().Cross(Vec3(1.0f, 0.0f, 0.0f));
				orbit->period = 100.0f;

				world->AddEntity(ent);
			}
			{
				NewEntity * ent = new NewEntity();

				PositionComponent * p = new PositionComponent();
				ColliderComponent * c = new ColliderComponent();
				GraphicsComponent * g = new GraphicsComponent(false);

				ent->addComponent(p);
				ent->addComponent(c);
				ent->addComponent(g);

				g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/planet_test.gmdl", Material("data/assets/terrain/textures/RockPlate.tga"), 0)));
				g->decs.items.front()->local *= 100.0f;
				g->decs.items.front()->local.data[15] = 1.0f;
				g->decs.items.front()->final = g->decs.items.front()->local;

				world->AddEntity(ent);
			}*/
			{
				NewEntity * ent = new NewEntity();

				SpawnComponent * spawn = new SpawnComponent();

				ent->addComponent(spawn);

				spawn->aabb_min = Vec3(21.0f, -10.0f, 21.0f);
				spawn->aabb_max = Vec3(31.0f, 40.0f, 30.0f);

				world->AddEntity(ent);
			}
		}

		// create ai entity
		for (int i = 0; i < 0; ++i)
		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent();
			p->p = Vec3(25.0f + 5.0f * i, 20.0f, 25.0f);
			GraphicsComponent * g = new GraphicsComponent();
			//AIComponent * ai = new AIComponent();
			//ai->random.seed(i);
			MobComponent * mob = new MobComponent();
			AnimationControlComponent * acc = new AnimationControlComponent();
			PoseComponent * pose = new PoseComponent();

			ent->addComponent(p);
			ent->addComponent(g);
			//ent->addComponent(ai);
			ent->addComponent(mob);
			ent->addComponent(acc);
			ent->addComponent(pose);

			/*for (int i = 1; i < 23; ++i)
			{
				LightComponent * light = new LightComponent();
				light->bone_id = i;
				ent->addComponent(light);
			}*/

			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/player/KnightGuy.gmdl", Material("data/assets/empty.tga")/*"data/assets/empty.tga"*/)));
			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/eyes/left.gmdl", Material("data/assets/decorators/eyes/basic.tga"))));
			g->decs.items.back()->priority = 1;
			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/eyes/right.gmdl", Material("data/assets/decorators/eyes/basic.tga"))));
			g->decs.items.back()->priority = 2;
			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/mouth/mouth.gmdl", Material("data/assets/decorators/mouth/neutral.tga"))));
			g->decs.items.back()->priority = 3;

			world->AddEntity(ent);
		}

		// create player entity
		{
			NewEntity * ent = new NewEntity();

			CameraControlComponent * cam = new CameraControlComponent();
			PositionComponent * p = new PositionComponent();
			GraphicsComponent * g = new GraphicsComponent();
			PlayerInputComponent * input = new PlayerInputComponent();
			MobComponent * mob = new MobComponent();
			//AnimationControlComponent * acc = new AnimationControlComponent();
			InventoryComponent * inv = new InventoryComponent();
			PoseComponent * pose = new PoseComponent();
			LineComponent * line = new LineComponent();

			ent->addComponent(cam);
			ent->addComponent(p);
			ent->addComponent(g);
			ent->addComponent(input);
			ent->addComponent(mob);
			//ent->addComponent(acc);
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

			//g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/player/KnightGuy.gmdl", Material("data/assets/units/player/KnightGuy.tga")/*"data/assets/empty.tga"*/)));
			//g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/eyes/left.gmdl", Material("data/assets/decorators/eyes/basic.tga"))));
			//g->decs.items.back()->priority = 1;
			//g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/eyes/right.gmdl", Material("data/assets/decorators/eyes/basic.tga"))));
			//g->decs.items.back()->priority = 2;
			//g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/mouth/mouth.gmdl", Material("data/assets/decorators/mouth/neutral.tga"))));
			//g->decs.items.back()->priority = 3;

			world->AddEntity(ent);

			input->client_id = data.client_id;
			cam->client_id = data.client_id;

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

#include "ScriptCode.h"

void main()
//INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//    PSTR lpCmdLine, INT nCmdShow)
{
	if (false)
	{
		size_t max_mem_size = 65536;
		void * mem = VirtualAlloc(nullptr, max_mem_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		while (true)
		{
			try
			{

				char buffer[4096];

				std::cin.getline(buffer, sizeof(buffer));

				ScriptCode code = ScriptCode(std::istringstream(buffer));

				ScriptCompile comp(mem);

				for (auto i = code.statements.begin(); i != code.statements.end(); ++i)
				{
					std::cout << i->output() << std::endl;
					i->compile(comp);
				}

				size_t mem_size = comp.ss.tellp();

				std::cout << "code bytes: " << mem_size << std::endl;

				comp.ss.read((char*)mem, mem_size);

				auto found_class = comp.classes.find("Main");
				if (found_class != comp.classes.end())
				{
					auto found_func = found_class->second->functions.find("main");
					if (found_func != found_class->second->functions.end())
					{
						typedef unsigned int func(unsigned int);

						func * f = static_cast<func*>(found_func->second.second);

						for (int i = 0; i < 20; ++i)
							std::cout << f(i) << std::endl;
					}
				}

			}
			catch (std::runtime_error& e)
			{
				std::cout << e.what() << std::endl;
			}
		}

		VirtualFree(mem, max_mem_size, MEM_RELEASE);
	}

	World * world = new World();
	Server * server = 0;
	Client * client = 0;
	world->use_chunks = false;
	char option;
	std::cout << "'p', 'c', 'h' or 's': ";
	std::cin >> option;
	if (option=='p')
	{
		client = new Client(world);
		server = new MyServer(world);
		server->onClientConnect(*client->clientData);
	}
	if (option=='c')
	{
		std::string address;
		unsigned int port;
		std::cout << "Address: ";
		std::cin >> address;
		std::cout << "Port: ";
		std::cin >> port;
		client = new Client(world);
		client->connect(address, port);
		server = new MyServer(world);
	}
	if (option == 'h')
	{
		unsigned int port;
		std::cout << "Port: ";
		std::cin >> port;
		client = new Client(world);
		server = new MyServer(world, port);
		server->onClientConnect(*client->clientData);
	}
	if (option=='s')
	{
		unsigned int port;
		std::cout << "Port: ";
		std::cin >> port;
		server = new MyServer(world, port);
	}
	world->client = client;
	world->server = server;
	if (server!=0)
	{
		server->client = client;
		if (client!=0)
			client->server = server;
		GameLoop gl(world, server, client);
		gl.init();
		if (client!=0)
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