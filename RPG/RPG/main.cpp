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
#include "ServiceComponent.h"
#include "InteractComponent.h"

#include "Item.h"

#include "ShadowSpawnUnit.h"
#include "GolemUnit.h"

#include "ShaderProgram.h"

#include "ClientData.h"

#include "Matrix3.h"

#include <regex>
#include <atomic>

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
		if (false)
		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent();
			ColliderComponent * c = new ColliderComponent();
			GraphicsComponent * g = new GraphicsComponent(false, 1);

			ent->addComponent(p);
			ent->addComponent(c);
			ent->addComponent(g);

			std::string ao = "data/assets/escape-ao.tga";

			Resource::load(ao, {"!sRGB" , "linear"});

			MaterialList materials;
			materials.materials.push_back(Material("data/assets/concrete_blue.tga"));
			materials.materials.push_back(Material("data/assets/asphalt_blue.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/plank.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/nground.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/RockPlate.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/brick2.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/RockPlate.tga"));
			materials.materials.push_back(Material("data/assets/chainlink.tga"));

			for (auto material = materials.materials.begin(); material != materials.materials.end(); ++material)
			{
				material->tex.push_back(ao);
			}

			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/escape.gmdl", materials, 0)));
			g->decs.items.front()->local *= 10.0f;
			g->decs.items.front()->local.mtrx[3][3] = 1.0f;
			g->decs.items.front()->final = g->decs.items.front()->local;

			world->AddEntity(ent);
		}

		// create level
		if (false)
		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent();
			ColliderComponent * c = new ColliderComponent();
			GraphicsComponent * g = new GraphicsComponent(false, 0);

			ent->addComponent(p);
			ent->addComponent(c);
			ent->addComponent(g);

			MaterialList materials;
			materials.materials.push_back(Material("data/assets/terrain/textures/RockPlate.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/ngrass.tga"));
			materials.materials.push_back(Material("data/assets/terrain/textures/nground.tga"));

			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/terrain/nworld.gmdl", materials, 0)));

			world->AddEntity(ent);
		}

		// create level
		if (true)
		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent();
			ColliderComponent * c = new ColliderComponent();
			GraphicsComponent * g = new GraphicsComponent(false, 1);

			ent->addComponent(p);
			ent->addComponent(c);
			ent->addComponent(g);

			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/terrain/tos.gmdl", MaterialList(), 0)));

			world->AddEntity(ent);
		}

		GolemUnit::spawn(Vec3(-50.0f, -300.0f, 0.0f), world);
		GolemUnit::spawn(Vec3(0.0f, 30.0f, -40.0f), world);
		GolemUnit::spawn(Vec3(50.0f, 90.0f, 20.0f), world);
		GolemUnit::spawn(Vec3(50.0f, -2115.0f, -215.0f + 0.5f), world);

		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent(Vec3(0.0f, 0.0f, -250.0f));
			GraphicsComponent * g = new GraphicsComponent(false, 4);

			ent->addComponent(p);
			ent->addComponent(g);

			Material water = "data/assets/water.tga";
			water.tex[0].options = { "mag_linear" };
			water.tex.push_back(TextureDescriptor("data/assets/water_normal.tga"));
			water.tex[1].options = { "cs_linear", "mag_linear" };

			MaterialList materials;
			materials.materials.push_back(water);
			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/water.gmdl", materials, 0)));

			world->AddEntity(ent);
		}

		// create wibbly wobbly wall
		//for (int j = 0; j < 2; ++j)
		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent(Vec3(-2.5f, 63.5f, -24.5f));
			ColliderComponent * c = new ColliderComponent();
			GraphicsComponent * g = new GraphicsComponent(false, 2);
			InteractComponent * i = new InteractComponent();

			ent->addComponent(p);
			ent->addComponent(c);
			ent->addComponent(g);
			ent->addComponent(i);

			Material wibbly = "data/assets/wibbly.tga";
			wibbly.tex[0].options = { "cs_linear", "mag_linear" };

			MaterialList materials;
			materials.materials.push_back(wibbly);
			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/plane_ds.gmdl", materials, 0)));
			g->decs.items.front()->local *= 3.0f;
			g->decs.items.front()->local.mtrx[3][3] = 1.0f;
			g->decs.items.front()->local *= Quaternion(Vec3(M_PI / 2.0f, 0.0f, 0.0f));
			g->decs.items.front()->final = g->decs.items.front()->local;

			i->name = "Shadow Wall";
			i->action_name = "Pass Through";

			i->func = [](MobComponent * user)
			{
				user->p->p += Vec3(0.0f, 1.0f, 0.0f);
			};

			world->AddEntity(ent);
		}

		for (size_t j = 0; j < 5; ++j)
		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent(Vec3(j, -2.5f, -0.5f));
			GraphicsComponent * g = new GraphicsComponent(false);
			InteractComponent * i = new InteractComponent();

			ent->addComponent(p);
			ent->addComponent(g);
			ent->addComponent(i);

			Material wibbly = "data/assets/white.tga";

			MaterialList materials;
			materials.materials.push_back(wibbly);
			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cube_bevel.gmdl", materials, 0)));
			g->decs.items.front()->local *= 0.1f;
			g->decs.items.front()->local.mtrx[3][3] = 1.0f;
			g->decs.items.front()->final = g->decs.items.front()->local;

			i->name = "Item";
			i->action_name = "Pick Up";

			i->func = [=](MobComponent * user)
			{
				auto inv = user->entity->getComponent<InventoryComponent>();
				if (inv)
				{
					auto item = std::make_shared<Item>();
					inv->items.add(item);
					inv->notifications.queue.push_back(*item);
					world->SetEntity(ent->id, nullptr);
				}
			};

			world->AddEntity(ent);
		}

		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent(Vec3(0.0f, 0.0f, -1.0f));
			ColliderComponent * c = new ColliderComponent();
			GraphicsComponent * g = new GraphicsComponent(false);
			InteractComponent * i = new InteractComponent();

			ent->addComponent(p);
			ent->addComponent(c);
			ent->addComponent(g);
			ent->addComponent(i);

			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/props/fountain.gmdl", MaterialList(), 0)));

			i->name = "Healing Fountain";
			i->action_name = "Drink";

			i->func = [=](MobComponent * user)
			{
				for (auto& res : user->resource)
				{
					res.current = res.max;
				}
			};

			world->AddEntity(ent);
		}

		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent();
			OrbitComponent * o = new OrbitComponent();
			ColliderComponent * c = new ColliderComponent();
			GraphicsComponent * g = new GraphicsComponent(false);

			o->center = Vec3(50.0f, -1115.0f, -235.0f);
			o->period = 200.0f;
			o->angle = Vec3(0.0f, 0.0f, 1.0f);
			o->offset = Vec3(0.0f, 1000.0f, 0.0f);
			o->t = -100.0f;

			ent->addComponent(p);
			ent->addComponent(o);
			ent->addComponent(c);
			ent->addComponent(g);

			Material wibbly = "data/assets/terrain/textures/RockPlate.tga";

			MaterialList materials;
			materials.materials.push_back(wibbly);
			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cube.gmdl", materials, 0)));
			g->decs.items.front()->local *= 20.0f;
			g->decs.items.front()->local.mtrx[3][3] = 1.0f;
			g->decs.items.front()->final = g->decs.items.front()->local;

			world->AddEntity(ent);
		}

		return;

		ShadowSpawnUnit::spawn(Vec3(0.0f, 30.0f, 0.0f), world);
		ShadowSpawnUnit::spawn(Vec3(-5.0f, 50.0f, 0.0f), world);
		ShadowSpawnUnit::spawn(Vec3(5.0f, 60.0f, 0.0f), world);

		// create grass
		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent(Vec3(-34.0f, -5.0f, -1.0f));
			GraphicsComponent * g = new GraphicsComponent(false, 3);

			ent->addComponent(p);
			ent->addComponent(g);

			MaterialList materials;
			materials.materials.push_back(Material("data/assets/grass_short.tga"));
			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/grass_wide.gmdl", materials, 0)));

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
				//game_state->setProgress(mob, progress);
			};

			world->AddEntity(ent);
		};

		auto createSpawner = [this](const GlobalPosition& aabb_min, const GlobalPosition& aabb_max)
		{
			NewEntity * ent = new NewEntity();

			SpawnComponent * spawn = new SpawnComponent();

			ent->addComponent(spawn);

			spawn->aabb_min = aabb_min;
			spawn->aabb_max = aabb_max;

			world->AddEntity(ent);

			return spawn;
		};

		auto createTicker = [this](std::function<void(void)> func, float delay)
		{
			NewEntity * ent = new NewEntity();

			ServiceComponent * service = new ServiceComponent();

			ent->addComponent(service);

			std::shared_ptr<float> countdown(new float(delay));

			service->on_tick = [func, countdown, delay](float dTime)
			{
				*countdown -= dTime;
				if (*countdown <= 0.0f)
				{
					func();
					*countdown = delay;
				}
			};

			return world->AddEntity(ent);
		};

		return;

		// create spawner
		{
			auto spawn = createSpawner(Vec3(21.0f, 20.0f, 21.0f), Vec3(41.0f, 40.0f, 30.0f));
			for (size_t i = 0; i < 20; i++)
			{
				spawn->spawn();
			}
		}

		// create spawner
		{
			auto spawn = createSpawner(Vec3(21.0f, -10.0f, 19.0f), Vec3(41.0f, 20.0f, 25.0f));
			for (size_t i = 0; i < 20; i++)
			{
				spawn->spawn();
			}
		}

		auto start_roof_spawn = createSpawner(Vec3(-20.0f, -9.0f, 32.0f), Vec3(-10.0f, 11.0f, 34.0f));
		for (size_t i = 0; i < 5; i++)
		{
			start_roof_spawn->spawn();
		}

		auto road_chase_riverside_spawn = createSpawner(Vec3(-45.0f, 60.0f, 0.0f), Vec3(-40.0f, 69.0f, 2.0f));
		auto road_tunnel_hill_spawn = createSpawner(Vec3(-100.0f, -110.0f, 20.0f), Vec3(-30.0f, -100.0f, 22.0f));

		auto road_hill_front_spawn = createSpawner(Vec3(70.0f, -100.0f, 20.0f), Vec3(80.0f, -110.0f, 22.0f));

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

			trigger->func_multi = [=](const std::vector<MobComponent*>& mobs)
			{
				for (size_t i = 0; i < 2; i++)
				{
					road_chase_riverside_spawn->spawn(Vec3(-1.0f, 0.0f, 0.0f));
				}
				for (size_t i = 0; i < 10; i++)
				{
					road_tunnel_hill_spawn->spawn(Vec3(0.0f, 1.0f, 0.0f));
				}
			};

			world->AddEntity(ent);
		}


		std::shared_ptr<float> road_hill_swarm_countdown(new float(0.0f));
		// create trigger volume
		{
			NewEntity * ent = new NewEntity();

			TriggerComponent * trigger = new TriggerComponent();

			ent->addComponent(trigger);

			trigger->aabb_min = Vec3(70.0f, -20.0f, 0.0f);
			trigger->aabb_max = Vec3(90.0f, 45.0f, 2.0f);

			trigger->delay = 1.0f;

			trigger->func = [=](MobComponent * mob)
			{
				*road_hill_swarm_countdown -= 1.0f;
				if (*road_hill_swarm_countdown <= 0.0f)
				{
					*road_hill_swarm_countdown += 100.0f;
					std::shared_ptr<float> duration(new float(20.0f));
					std::shared_ptr<int> ticker_id(new int(-1));
					*ticker_id = createTicker([=]()
					{
						for (size_t i = 0; i < 5; i++)
						{
							road_hill_front_spawn->spawn(Vec3(0.0f, 1.0f, 0.0f));
						}
						*duration -= 1.0f;
						if (*duration <= 0.0f)
						{
							world->SetEntity(*ticker_id, nullptr);
						}
					}, 1.0f);
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
		createProgressZone(Vec3(-100.0f, -160.0f, 0.0f), Vec3(-80.0f, -140.0f, 2.0f), 1.0f);
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

	void onClientDisconnect(ClientData& data)
	{
		auto chats = world->GetComponents<ChatComponent>();
		if (chats.size())
		{
			ChatMessage message;
			message.message = "Client id " + std::to_string(data.client_id) + " disconnected.\n";
			message.timeout = 10.0f;
			chats.front()->messages.push_back(message);
		}
		auto game_states = world->GetComponents<GameStateComponent>();
		if (game_states.size())
		{
			//game_states.front()->removePlayer(data.client_id);
		}
	}
};

#include "SteamWrapper.h"

#include "ScriptCode.h"

#include "StringResource.h"

class TestClass
{
public:
	virtual ~TestClass() {}

	virtual uint64_t func(uint64_t x) { return 0; };

	void print(uint64_t ui64)
	{
		std::cout << ui64;
	}

	void println(uint64_t ui64)
	{
		std::cout << ui64 << std::endl;
	}

	uint64_t a;
};

void start_engine_instance(std::string address, uint16_t port, uint64_t lobby_id, char option, std::atomic<bool>& running)
{
	World * world = new World();
	Server * server = nullptr;
	Client * client = nullptr;

	if (option == 'p')
	{
		server = new MyServer(world);
		client = new Client(world);
		server->onServerActivated();
		server->onClientConnect(*client->clientData);
	}
	if (option == 'c')
	{
		server = new MyServer(world);
		client = new Client(world);
		client->connect(address, port);
	}
	if (option == 'h')
	{
		server = new MyServer(world);
		client = new Client(world);
		server->open(port);
		server->onServerActivated();
		server->onClientConnect(*client->clientData);
	}
	if (option == 's')
	{
		server = new MyServer(world);
		server->open(port);
		server->onServerActivated();
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
			while (client->IsAlive() && running)
				gl.tick();
		}
		else
		{
			while (running)
				gl.tick();
		}
	}
}

#include "ItemType.h"

//INT WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
//	PSTR lpCmdLine, INT nCmdShow)
int main()
{
	if (false)
	{
		size_t max_mem_size = 65536;
		void * mem = VirtualAlloc(nullptr, max_mem_size, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);

		while (true)
		{
			//try
			{
				char buffer[4096];

				std::cin.getline(buffer, sizeof(buffer));

				//ScriptCode code = ScriptCode(std::istringstream(buffer));

				auto file = Resource::get<StringResource>("text2.txt", { "block", "reload" });

				ScriptCode code = ScriptCode(std::istringstream(file->string));

				ScriptCompile comp(mem);

				std::shared_ptr<ScriptClassData> test_class_data(new ScriptClassData());
				test_class_data->class_name = "TestClass";
				test_class_data->AddVirtualFunctionTable();

				ScriptTypeData a_data = NewScriptTypeData<decltype(TestClass::a)>();
				test_class_data->AddMember("a", a_data, offsetof(TestClass, a));

				ScriptFunctionPrototype destructor_data;
				destructor_data.cc = CC_MICROSOFT_X64;
				test_class_data->AddVirtualFunction("~", destructor_data);
				ScriptFunctionPrototype func_data;
				func_data.cc = CC_MICROSOFT_X64;
				func_data.ret = a_data;
				func_data.params.push_back(a_data);
				test_class_data->AddVirtualFunction("func", func_data);

				test_class_data->AddFunction("print", &TestClass::print);
				test_class_data->AddFunction("println", &TestClass::println);

				comp.classes.insert(std::make_pair("TestClass", test_class_data));

				for (auto i = code.statements.begin(); i != code.statements.end(); ++i)
				{
					std::cout << i->output() << std::endl;
					i->compile(comp);
				}

				comp.Compile();
				comp.Link();

				size_t mem_size = comp.ss.tellp();

				std::cout << "code bytes: " << mem_size << std::endl;

				comp.ss.read((char*)mem, mem_size);

				std::string class_name = "Main";
				auto found_class = comp.classes.find(class_name);
				if (found_class != comp.classes.end())
				{
					auto found_constructor = found_class->second->functions.find(class_name + "::" + class_name);
					if (found_constructor != found_class->second->functions.end())
					{
						TestClass * t = (TestClass*)operator new(found_class->second->size);

						void (*func)(TestClass*) = (void(*)(TestClass*))found_constructor->second.second;
						func(t);

						for (int i = 0; i < 100; ++i)
							std::cout << t->func(i) << std::endl;

						delete t;
					}
				}

			}
			/*catch (std::runtime_error& e)
			{
				std::cout << e.what() << std::endl;
			}*/
		}

		VirtualFree(mem, max_mem_size, MEM_RELEASE);
	}

	std::string address;
	uint16_t port = 7777;
	uint64_t lobby_id = 0;
	char option = 'h';

	std::cout << "starting game..." << std::endl;

	/*std::cmatch match_server;
	std::regex reg_server("\\+server (.*)");
	if (std::regex_match(lpCmdLine, match_server, reg_server))
	{
		option = 's';
		port = std::atoi(match_server[1].str().c_str());
	}

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
	std::regex_match(lpCmdLine, match_connect_lobby, reg_connect_lobby);*/

	//std::shared_ptr<ISteamWrapper> steam(ISteamWrapper::make());
	
	ItemType::init();

	std::atomic<bool> running = true;
	if (option == 'h')
	{
		std::thread t([&]()
		{
			start_engine_instance(address, port, lobby_id, 's', running);
		});
		start_engine_instance("127.0.0.1", port, lobby_id, 'c', running);
		running = false;
		t.join();
	}
	else
	{
		start_engine_instance(address, port, lobby_id, option, running);
	}

	ItemType::release();

	Resource::unload();
}