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
	is >> team_selection;
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

	if (setting_up)
	{
		entity->world->cam_pos = Vec3(50.0f, 50.0f, 50.0f);
		entity->world->cam_rot = Quaternion(-M_PI * 0.25f, Vec3(0.0f, 0.0f, 1.0f)) * Quaternion(M_PI * 0.75f, Vec3(1.0f, 0.0f, 0.0f));
	}
}

void GameStateComponent::post_frame(float dTime)
{
	Client * client = entity->world->client;
	if (setting_up)
	{
		if (client->input.isPressed(Platform::KeyEvent::A))
			team_selection = 0;
		if (client->input.isPressed(Platform::KeyEvent::D))
			team_selection = 1;
		if (entity->world->authority)
		{
			addPlayer(client->clientData->client_id, team_selection);
			if (client->input.isPressed(Platform::KeyEvent::SPACE))
				startGame();
		}
	}
}

void GameStateComponent::tick(float dTime)
{
	if (entity->world->authority)
	{
		if (setting_up)
		{
			startGame();
		}

		if (game_over)
		{
			countdown -= dTime;
			if (countdown <= 0.0f)
			{
				game_over = false;
				setting_up = true;
				teams[0].clearProgress();
				swapTeams();

				if (team_selection != 0xffffffff)
					team_selection = 1 - team_selection;

				set_display(false);

				GameStateComponent * copy = new GameStateComponent(*this);
				World * world = entity->world;
				entity->world->server->reset([copy, world]()
				{
					NewEntity * ent = new NewEntity();

					ent->addComponent(copy);

					world->AddEntity(ent);
				});
			}
		}

		if (!setting_up && !game_over)
		{
			survivor_progress = getMaximumSurvivorProgress();

			if (getMinimumAliveSurvivorProgress() == 1.0f)
			{
				game_over = true;
				countdown = 10.0f;
				teams[0].score += 100 * getAverageSurvivorProgress();
			}

			auto mobs = entity->world->GetComponents<MobComponent>();
			mobs.erase(std::remove_if(mobs.begin(), mobs.end(), [](MobComponent * mob)
			{
				return mob->temp_team == 1;
			}), mobs.end());
			if (mobs.empty())
			{
				game_over = true;
				countdown = 10.0f;
			}
		}
	}
}

void GameStateComponent::writeLog(outstream& os, ClientData& client)
{
	bool client_setting_up = setting_up || getPlayerTeam(client.client_id) == 0xffffffff;
	os << client_setting_up;
	if (!client_setting_up)
	{
		os << game_over;
		os << survivor_progress;
		if (game_over)
		{
			os << countdown;
			//os << teams[0].score;
		}
	}
}

void GameStateComponent::readLog(instream& is)
{
	is >> setting_up;
	if (!setting_up)
	{
		is >> game_over;
		is >> survivor_progress;
		if (game_over)
		{
			is >> countdown;
			//is >> teams[0].score;
		}
	}
	else
	{
		game_over = false;
	}
}

void GameStateComponent::writeLog(outstream& os)
{
	if (setting_up)
	{
		if (team_selection != 0xffffffff)
		{
			os << team_selection;
		}
	}
}

void GameStateComponent::readLog(instream& is, ClientData& client)
{
	int32_t client_team;
	is >> client_team;
	if (setting_up)
	{
		addPlayer(client.client_id, client_team);
	}
}

void GameStateComponent::interpolate(Component * pComponent, float fWeight)
{
	GameStateComponent * other = reinterpret_cast<GameStateComponent*>(pComponent);

	setting_up = other->setting_up;
	game_over = other->game_over;
	survivor_progress = other->survivor_progress;
	countdown = other->countdown;
}

void GameStateComponent::write_to(outstream& os, ClientData& client) const
{
	os << getPlayerTeam(client.client_id);
}

void GameStateComponent::write_to(outstream& os) const
{
}

void GameStateComponent::startGame(void)
{
	if (setting_up && !teams[0].members.empty())
	{
		setting_up = false;

		for (size_t i = 0; i < teams.size(); i++)
		{
			size_t index = 0;
			teams[i].progress.resize(teams[i].members.size());
			for (auto member = teams[i].members.begin(); member != teams[i].members.end(); ++member)
			{
				teams[i].progress[index] = 0.0f;
				member->second = createAvatar(member->first, i, index++);
			}
		}
	}
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

	if (team == 0)
	{
		p->p = Vec3(-15.0f, -5.0f, 23.0f);
		//pose->anim = "data/assets/units/player/hoodlum.anim";
		//g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/player/hoodlum.gmdl", Material("data/assets/units/player/hoodlum.tga"))));
		pose->anim = "data/assets/units/golem/golem.anim";
		MaterialList materials;
		materials.materials.push_back(Material("data/assets/black.tga"));
		materials.materials.push_back(Material("data/assets/units/golem/golem.tga"));
		materials.materials.back().tex.push_back("data/assets/units/golem/golem_ao.tga");
		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/golem/golem.gmdl", materials)));
		g->tag = 1;
	}
	if (team == 1)
	{
		p->p = Vec3(-15.0f, -5.0f, 33.0f);
		pose->anim = "data/assets/units/player/KnightGuy.anim";
		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/player/KnightGuy.gmdl", Material("data/assets/terrain/textures/nground.tga"))));
	}
	p->p += Vec3(0.0f, 1.0f, 0.0f) * index;
	mob->temp_team = team;


	if (team == 0)
	{
		mob->on_death = [=]()
		{
			teams[0].members[index].second = nullptr;
		};

		mob->on_tick = [=](float dTime)
		{
			if (mob->input["attack"])
			{
				if (mob->weapon_index == 0)
				{
					//auto spawn_bullet = [=](const Vec3& muzzle_velocity)
					//{
					//	NewEntity * ent = new NewEntity();

					//	PositionComponent * pos = new PositionComponent();
					//	ProjectileComponent * projectile = new ProjectileComponent();
					//	GraphicsComponent * g = new GraphicsComponent(false);

					//	ent->addComponent(pos);
					//	ent->addComponent(projectile);
					//	ent->addComponent(g);

					//	projectile->v = muzzle_velocity + mob->v;
					//	projectile->drag = 0.01f;

					//	pos->p = *mob->p + mob->up * 0.45f;

					//	projectile->on_collision = [=](MobComponent * target)
					//	{
					//		if (target)
					//		{
					//			target->do_damage(20, mob->entity->get_id());
					//			target->hit = true;
					//		}
					//		//ent->world->SetEntity(ent->id, nullptr);
					//		projectile->v = Vec3();
					//	};

					//	projectile->shooter = mob;

					//	g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cube.gmdl", Material("data/assets/empty.tga"), 0)));
					//	g->decs.items.front()->local *= 0.0127f * 0.5f;
					//	g->decs.items.front()->local.data[15] = 1.0f;
					//	g->decs.items.front()->local *= mob->cam_rot;

					//	mob->entity->world->AddEntity(ent);
					//};

					//spawn_bullet(mob->cam_facing * 470.0f);

					//mob->weapon->recoil += 0.3f;

					//NewEntity * sound_ent = new NewEntity();
					//auto audio = new AudioComponent("data/assets/audio/bang.wav");
					//audio->pos_id = mob->weapon->entity->get_id();
					//sound_ent->addComponent(audio);
					//mob->entity->world->AddEntity(sound_ent);

					if (acc->state != 4 && mob->landed && mob->input.find("recover") == mob->input.end())
					{
						acc->set_state(4);

						NewEntity * sound_ent = new NewEntity();
						auto audio = new AudioComponent("data/assets/audio/hya.wav");
						audio->pos_id = ent->get_id();
						sound_ent->addComponent(audio);
						ent->world->AddEntity(sound_ent);

						auto nearby_mobs = ent->world->GetNearestComponents<MobComponent>(p->p + mob->facing * 1.75f, 1.75f);
						for each (auto nearby in nearby_mobs)
						{
							if (nearby.second->temp_team != mob->temp_team)
							{
								std::vector<std::shared_ptr<Collision>> list;
								ColliderComponent::LineCheck(p->p, *nearby.second->p, list);
								if (list.empty())
								{
									nearby.second->do_damage(4, ent->get_id());
									nearby.second->hit = true;
									Vec3 dif = *nearby.second->p - p->p;
									dif.Normalize();
									nearby.second->v = dif * 8.0f + Vec3(0.0f, 0.0f, 1.0f);
									nearby.second->input["rolling"] += 0.2f;
								}
							}
						}

						mob->stamina.current -= 1;
						mob->v = Vec3();
						mob->input["rolling"] = 0.4f;
						mob->input.erase("attack");
					}
				}

				if (mob->weapon_index == 1)
				{
					if (mob->mana.current > 0.0f)
					{
						mob->do_heal(std::fminf(4.0f, mob->mana.current), mob->entity->get_id());
						mob->mana.current -= 4.0f;
					}

					mob->input.erase("attack");
				}
			}
		};
	}

	if (team == 1)
	{
		mob->on_death = [=]()
		{
			createAvatar(client_id, team, index);
		};

		/*mob->attack = [mob]()
		{
			if (mob->input.find("attack_cooldown") == mob->input.end())
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
					projectile->drag = 0.04f;

					projectile->on_collision = [ent, pos](MobComponent * target)
					{
						if (target)
						{
							target->stamina.current = 0.0f;
						}
						ent->world->SetEntity(ent->id, nullptr);
					};

					g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cube.gmdl", Material("data/assets/terrain/textures/ngrass.tga"), 0)));
					g->decs.items.front()->local *= 0.1f;
					g->decs.items.front()->local.data[15] = 1.0f;
					g->decs.items.front()->local *= mob->cam_rot;

					mob->entity->world->AddEntity(ent);
				};

				spawn_bullet(mob->cam_facing * 40.0f);

				mob->input.erase("attack");
				mob->input["attack_cooldown"] = 5.0f;
			}
		};*/

		mob->on_tick = [=](float dTime)
		{
			if (mob->input.find("attack") != mob->input.end())
			{
				if (mob->input.find("attack_cooldown") == mob->input.end())
				{
					auto nearby_mobs = entity->world->GetNearestComponents<MobComponent>(*mob->p + mob->cam_facing * 1.0f, 1.0f);
					MobComponent * other = nullptr;
					for each (auto nearby in nearby_mobs)
					{
						if (nearby.second != mob)
						{
							std::vector<std::shared_ptr<Collision>> list;
							ColliderComponent::LineCheck(*mob->p, *nearby.second->p, list);
							if (list.empty())
							{
								other = nearby.second;
								break;
							}
						}
					}

					if (other != nullptr)
					{
						other->hit = true;
						other->health.current -= 15.0f;
						Vec3 flat_facing = mob->cam_facing;
						flat_facing.z = 0.0f;
						flat_facing.Normalize();
						other->v += flat_facing * 10.0f + Vec3(0.0f, 0.0f, 4.0f);
					}

					mob->input.erase("attack");
					mob->input["attack_cooldown"] = 0.5f;

					NewEntity * sound_ent = new NewEntity();
					auto audio = new AudioComponent("data/assets/audio/bang.wav");
					audio->pos_id = mob->entity->get_id();
					sound_ent->addComponent(audio);
					mob->entity->world->AddEntity(sound_ent);

					acc->set_state(0);
					acc->set_state(3);
				}
			}
		};

		/*mob->input["slime"] = 10.0f;
		mob->on_tick = [=](float dTime)
		{
			if (mob->input["attack"] && !mob->input["spraying"])
			{
				mob->input["spraying"] = 1.0f;
				mob->input["windup"] = 0.7f;
				mob->input.erase("attack");

				NewEntity * sound_ent = new NewEntity();
				auto audio = new AudioComponent("data/assets/audio/ZombieSpit.wav");
				audio->pos_id = ent->get_id();
				sound_ent->addComponent(audio);
				mob->entity->world->AddEntity(sound_ent);
			}

			if (mob->input["spraying"])
			{
				mob->input["spraying"] = 1.0f;
				if (!mob->input["windup"])
				{
					if (!mob->input["spray_delay"])
					{
						mob->input["spray_delay"] = 0.25f;
						mob->input["slime"] -= 0.5f;

						auto spawn_bullet = [mob](const Vec3& muzzle_velocity)
						{
							NewEntity * ent = new NewEntity();

							PositionComponent * pos = new PositionComponent();
							ProjectileComponent * projectile = new ProjectileComponent();
							GraphicsComponent * g = new GraphicsComponent(false);

							ent->addComponent(pos);
							ent->addComponent(projectile);
							ent->addComponent(g);

							pos->p = *mob->p + mob->up * 0.45f + Vec3(0.0f, -0.2f, 0.0f) * mob->cam_rot;

							projectile->v = muzzle_velocity + mob->v;
							projectile->drag = 0.04f;

							projectile->on_collision = [=](MobComponent * target)
							{
								if (target)
								{
									target->input["slime"] += 0.5f;
									if (target != mob)
										target->do_damage(target->input["slime"], mob->entity->get_id());
								}
								ent->world->SetEntity(ent->id, nullptr);
							};

							projectile->shooter = mob;

							g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/cube.gmdl", Material("data/assets/terrain/textures/ngrass.tga"), 0)));
							g->decs.items.front()->local *= 0.05f;
							g->decs.items.front()->local.data[15] = 1.0f;
							g->decs.items.front()->local *= mob->cam_rot;

							mob->entity->world->AddEntity(ent);
						};

						spawn_bullet(mob->cam_facing * 20.0f);

						NewEntity * sound_ent = new NewEntity();
						auto audio = new AudioComponent("data/assets/audio/ZombieSpitContinue.wav");
						audio->pos_id = ent->get_id();
						sound_ent->addComponent(audio);
						mob->entity->world->AddEntity(sound_ent);

						if (mob->input["slime"] <= 0.0f)
						{
							mob->input.erase("spraying");
						}
					}
				}
			}

			mob->input["slime"] += dTime;
			if (!mob->input["spraying"])
				mob->input["slime"] += dTime * 2.5f;
			if (mob->input["slime"] > 10.0f)
				mob->input["slime"] = 10.0f;
		};*/
	}

	input->client_id = client_id;
	cam->client_id = client_id;
	inv->client_id = client_id;

	entity->world->AddEntity(ent);
	
	mob->p = &p->p;

	if (team == 0)
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
		w->mob_id = mob->entity->id;

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
	for (auto team = teams.begin(); team != teams.end(); ++team)
	{
		for (auto member = team->members.begin(); member != team->members.end(); ++member)
		{
			if (member->first == client_id)
			{
				team->members.erase(member);
				return;
			}
		}
	}
}

void GameStateComponent::swapTeams(void)
{
	auto temp = teams[0];
	teams[0] = teams[1];
	teams[1] = temp;
}

void GameStateComponent::setProgress(MobComponent * mob, float progress)
{
	teams[0].setProgress(mob, progress);
}

float GameStateComponent::getAverageSurvivorProgress(void)
{
	float total = 0.0f;
	for each (auto progress in teams[0].progress)
	{
		total += progress;
	}
	total /= teams[0].progress.size();
	return total;
}

float GameStateComponent::getMaximumSurvivorProgress(void)
{
	float total = 0.0f;
	for each (auto progress in teams[0].progress)
	{
		total = std::fmaxf(progress, total);
	}
	return total;
}

float GameStateComponent::getMinimumAliveSurvivorProgress(void)
{
	float total = 1.0f;
	for (size_t i = 0; i < teams[0].progress.size(); i++)
	{
		if (teams[0].members[i].second)
			total = std::fminf(teams[0].progress[i], total);
	}
	return total;
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
							if (entity->world->authority)
							{
								if (teams[0].members.empty())
									Writing::render("There needs to be at least one survivor to start the game.", rs);
								else
									Writing::render("Press space to start the game.", rs);
							}
							else
							{
								Writing::render("Waiting for the game to start.", rs);
							}
						}
						rs.popTransform();
						rs.popTransform();
					}

					if (!setting_up && !game_over)
					{
						rs.pushTransform();
						rs.addTransform(Matrix4::Translation(Vec3(400.0f, 100.0f, 0.0f)));
						Writing::setSize(30);
						Writing::setColor(0.9f, 0.9f, 0.9f);
						Writing::render("Survivor progress: " + std::to_string((int)(survivor_progress * 100.0f)) + "%", rs);
						rs.popTransform();
						rs.popTransform();
					}

					if (game_over)
					{
						rs.pushTransform();
						rs.addTransform(Matrix4::Translation(Vec3(400.0f, 600.0f, 0.0f)));
						Writing::setSize(50);
						Writing::setColor(0.9f, 0.9f, 0.9f);
						Writing::render("Game Over!\n", rs);
						Writing::setSize(30);
						Writing::render("Restarting in " + std::to_string((int)countdown), rs);
						rs.popTransform();
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
