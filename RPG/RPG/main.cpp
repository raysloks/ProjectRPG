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

#include "ClientData.h"

#include "Matrix3.h"

class MyServer :
	public Server
{
public:
	MyServer(World * pWorld, unsigned short port) : Server(pWorld, port) {}
	MyServer(World * pWorld) : Server(pWorld) {}

	void connectClientToEntity(ClientData& data)
	{
		if (world->units.size()==0)
		{
			{
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
			}
			{
				NewEntity * ent = new NewEntity();

				PositionComponent * p = new PositionComponent();
				ColliderComponent * c = new ColliderComponent();
				GraphicsComponent * g = new GraphicsComponent(false);

				ent->addComponent(p);
				ent->addComponent(c);
				ent->addComponent(g);

				std::vector<std::string> tex;
				tex.push_back("data/assets/terrain/textures/grass.tga");
				tex.push_back("data/assets/terrain/textures/RockPlate.tga");
				tex.push_back("data/assets/terrain/textures/temple.tga");

				g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/terrain/test/low.gmdl", tex, 0)));
				g->decs.items.front()->local *= 600.0f;
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
			for (int i = 0; i < 20; ++i)
			{
				NewEntity * ent = new NewEntity();

				PositionComponent * p = new PositionComponent();
				p->p += Vec3(0.2f * i, 0.015f * i, 0.15f * i - 12.0f);
				ColliderComponent * c = new ColliderComponent();
				GraphicsComponent * g = new GraphicsComponent(false);

				ent->addComponent(p);
				ent->addComponent(c);
				ent->addComponent(g);

				std::vector<std::string> tex;
				tex.push_back("data/assets/terrain/textures/grass.tga");

				g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cube.gmdl", tex, 0)));
				g->decs.items.front()->local *= 0.1f;
				g->decs.items.front()->local.mtrx[3][3] = 1.0f;
				g->decs.items.front()->final = g->decs.items.front()->local;

				world->AddEntity(ent);
			}
		}

		NewEntity * ent = new NewEntity();

		PositionComponent * p = new PositionComponent();
		GraphicsComponent * g = new GraphicsComponent();
		PlayerInputComponent * input = new PlayerInputComponent();
		MobComponent * mob = new MobComponent();
		CameraControlComponent * cam = new CameraControlComponent();
		AnimationControlComponent * acc = new AnimationControlComponent();
		//InventoryComponent * inv = new InventoryComponent();
		HitComponent * hit = new HitComponent();
		LineComponent * line = new LineComponent();

		ent->addComponent(p);
		ent->addComponent(g);
		ent->addComponent(input);
		ent->addComponent(mob);
		ent->addComponent(cam);
		ent->addComponent(acc);
		//ent->addComponent(inv);
		ent->addComponent(hit);
		ent->addComponent(line);

		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/player/KnightGuy.gmdl", "data/assets/units/player/KnightGuy.tga"/*"data/assets/empty.tga"*/)));
		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/eyes/left.gmdl", "data/assets/decorators/eyes/basic.tga")));
		g->decs.items.back()->priority = 1;
		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/eyes/right.gmdl", "data/assets/decorators/eyes/basic.tga")));
		g->decs.items.back()->priority = 2;
		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/mouth/mouth.gmdl", "data/assets/decorators/mouth/neutral.tga")));
		g->decs.items.back()->priority = 3;
		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/items/weapons/swords/claymore.gmdl", "data/assets/items/weapons/swords/claymore.tga", 15)));

		data.unit_id = world->AddEntity(ent);
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

				ScriptCompile comp;

				comp.proto.ret.size = 4;
				comp.proto.ret.type = ST_UINT;

				//comp.stack = sizeof(int);

				for (auto i = code.statements.begin(); i != code.statements.end(); ++i)
				{
					std::cout << i->output() << std::endl;
					i->compile(comp);
				}

				auto& ss = comp.ss;

				size_t mem_size = ss.tellp();

				std::cout << "code bytes: " << mem_size << std::endl;

				ss.read((char*)mem, mem_size);

				typedef int func(int);

				func * f = static_cast<func*>(mem);

				std::cout << f(300) << std::endl;

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