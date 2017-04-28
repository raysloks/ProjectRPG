#include "GameStateComponent.h"

#include "World.h"

#include "Client.h"
#include "ClientData.h"

#include "RenderSetup.h"

#include "Writing.h"

#include "TeamData.h"

const AutoSerialFactory<GameStateComponent> GameStateComponent::_factory("GameStateComponent");

GameStateComponent::GameStateComponent(void) : Serializable(_factory.id)
{
	setting_up = true;
	game_over = false;
	team_selection = 0xffffffff;

	teams.resize(2);
}

GameStateComponent::GameStateComponent(instream& is, bool full) : Serializable(_factory.id)
{
	setting_up = true;
	game_over = false;
	team_selection = 0xffffffff;
}

GameStateComponent::~GameStateComponent(void)
{
	set_display(false);
}

void GameStateComponent::connect(NewEntity * pEntity, bool authority)
{
}

void GameStateComponent::disconnect(void)
{
}

void GameStateComponent::pre_frame(float dTime)
{
	set_display(true);

	if (setting_up)
	{
		entity->world->cam_pos = Vec3(50.0f, 50.0f, 50.0f);
		entity->world->cam_rot = Quaternion(-M_PI * 0.25f, Vec3(0.0f, 0.0f, 1.0f)) * Quaternion(M_PI * 0.75f, Vec3(1.0f, 0.0f, 0.0f));
	}
}

void GameStateComponent::post_frame(float dTime)
{
	Client * client = entity->world->client;
	if (client->input.isPressed(Platform::KeyEvent::A))
		team_selection = 0;
	if (client->input.isPressed(Platform::KeyEvent::D))
		team_selection = 1;
}

void GameStateComponent::tick(float dTime)
{
	if (!setting_up && !game_over)
	{
		//check for end of game
	}
}

void GameStateComponent::writeLog(outstream& os, ClientData& client)
{
	bool client_setting_up = setting_up || getPlayerTeam(client.client_id) == 0xffffffff;
	os << client_setting_up;
	if (client_setting_up)
	{
		os << game_over;
	}
}

void GameStateComponent::readLog(instream& is)
{
	is >> setting_up;
	if (!setting_up)
	{
		is >> game_over;
	}
	else
	{
		game_over = false;
	}
}

void GameStateComponent::writeLog(outstream& os)
{
	if (setting_up && team_selection != 0xffffffff)
		os << team_selection;
}

void GameStateComponent::readLog(instream& is, ClientData& client)
{
	int32_t client_team;
	is >> client_team;
	addPlayer(client.client_id, client_team);
}

void GameStateComponent::interpolate(Component * pComponent, float fWeight)
{
	GameStateComponent * other = reinterpret_cast<GameStateComponent*>(pComponent);

	setting_up = other->setting_up;
	game_over = other->game_over;
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

void GameStateComponent::startGame(void)
{
	if (setting_up && !teams[0].members.empty())
	{
		setting_up = false;

		for (size_t i = 0; i < teams.size(); i++)
		{
			for each (auto member in teams[i].members)
			{
				createAvatar(member.first, i);
			}
		}
	}
}

MobComponent * GameStateComponent::createAvatar(uint32_t client_id, uint32_t team)
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

	p->p = Vec3(-15.0f, -5.0f, 23.0f);

	g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/player/KnightGuy.gmdl", Material("data/assets/units/player/KnightGuy.tga"), 0)));

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

	input->client_id = client_id;
	cam->client_id = client_id;
	inv->client_id = client_id;

	entity->world->AddEntity(ent);

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

		entity->world->AddEntity(ent);
	}

	return mob;
}

uint32_t GameStateComponent::getPlayerTeam(uint32_t client_id) const
{
	for (size_t i = 0; i < teams.size(); i++)
	{
		for each (auto member in teams[i].members)
		{
			if (member.first == client_id)
				return i;
		}
	}
	return 0xffffffff;
}

void GameStateComponent::addPlayer(uint32_t client_id, uint32_t team)
{
	auto current_team = getPlayerTeam(client_id);
	if (current_team != team)
	{
		if (current_team != 0xffffffff)
			removePlayer(client_id);
		if (team < teams.size())
		{
			teams[team].members.push_back(std::pair<uint32_t, MobComponent*>(client_id, nullptr));
		}
	}
}

void GameStateComponent::removePlayer(uint32_t client_id)
{
	for each (auto team in teams)
	{
		for (auto member = team.members.begin(); member != team.members.end(); ++member)
		{
			if (member->first == client_id)
			{
				team.members.erase(member);
				return;
			}
		}
	}
}

void GameStateComponent::set_display(bool enable)
{
	Client * client = entity->world->client;
	if (client != nullptr)
	{
		if (enable)
		{
			if (!func)
			{
				func.reset(new std::function<void(RenderSetup&)>([this](RenderSetup& rs) {

					rs.pushTransform();
					rs.addTransform(Matrix4::Translation(Vec3(400.0f, 200.0f, 0.0f)));
					Writing::setSize(30);
					Writing::setColor(0.9f, 0.9f, 0.9f);
					//Writing::render("Your score : 124\nEnemy Score : 104", rs);
					//rs.popTransform();
					rs.popTransform();

					if (setting_up)
					{
						rs.pushTransform();
						rs.addTransform(Matrix4::Translation(Vec3(400.0f, 600.0f, 0.0f)));
						Writing::setSize(30);
						Writing::setColor(0.9f, 0.9f, 0.9f);
						if (team_selection == 0xffffffff)
						{
							Writing::render("Select a Team!\nA to select Survivors. D to select Zombies.", rs);
						}
						else
						{
							Writing::render("You have joined the ", rs);
							if (team_selection == 0)
							{
								Writing::setColor(0.8f, 0.4f, 0.3f);
								Writing::render("Survivors", rs);
								rs.popTransform();
							}
							if (team_selection == 1)
							{
								Writing::setColor(0.4f, 0.8f, 0.3f);
								Writing::render("Zombies", rs);
								rs.popTransform();
							}
							rs.popTransform();
							rs.addTransform(Matrix4::Translation(Vec3(0.0f, 40.0f, 0.0f)));
							Writing::setColor(0.9f, 0.9f, 0.9f);
							Writing::render("Waiting for game to start.", rs);
						}
						rs.popTransform();
						rs.popTransform();
					}

					if (game_over)
					{
						rs.pushTransform();
						rs.addTransform(Matrix4::Translation(Vec3(400.0f, 600.0f, 0.0f)));
						Writing::setSize(50);
						Writing::setColor(0.9f, 0.9f, 0.9f);
						Writing::render("Game Over!", rs);
						rs.popTransform();
						rs.popTransform();
					}

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
