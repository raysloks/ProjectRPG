#include "GolemUnit.h"

#include "NewEntity.h"
#include "World.h"

#include "PositionComponent.h"
#include "GraphicsComponent.h"
#include "AIComponent.h"
#include "MobComponent.h"
#include "AnimationControlComponent.h"
#include "PoseComponent.h"
#include "HitComponent.h"
#include "ProjectileComponent.h"
#include "ChatComponent.h"

#include "SimpleState.h"

#include "ShadowSpawnUnit.h"

class GolemAI :
	public AIWrapper
{
public:
	void tick(float dTime);

	World * world;

	PositionComponent * p;
	MobComponent * mob;
	AnimationControlComponent * acc;

	std::shared_ptr<std::function<void(HitData&)>> on_death;

	EntityID target;
	float delay;
};

MobComponent * GolemUnit::spawn(const GlobalPosition& pos, World * world)
{
	NewEntity * entity = new NewEntity();

	PositionComponent * p = new PositionComponent();
	p->p = pos;
	GraphicsComponent * g = new GraphicsComponent();
	AIComponent * ai = new AIComponent();
	MobComponent * mob = new MobComponent();
	AnimationControlComponent * acc = new AnimationControlComponent();
	PoseComponent * pose = new PoseComponent();

	entity->addComponent(p);
	entity->addComponent(g);
	entity->addComponent(ai);
	entity->addComponent(mob);
	entity->addComponent(acc);
	entity->addComponent(pose);

	pose->anim = "data/assets/units/golem/golem.anim";
	MaterialList materials;
	materials.materials.push_back(Material("data/assets/black.tga"));
	materials.materials.push_back(Material("data/assets/units/golem/golem.tga"));
	materials.materials.back().tex.push_back(TextureDescriptor("data/assets/units/golem/golem_ao.tga"));
	g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/golem/golem.gmdl", materials)));
	//g->tag = 1;

	acc->scale = 3.0f;

	world->AddEntity(entity);

	mob->p = p;

	mob->r = 1.0f;
	mob->use_base_collision = false;

	mob->temp_team = 1;

	mob->health = ResourceBar(100);
	mob->stamina = ResourceBar(50);
	mob->mana = ResourceBar(50);

	auto wrapper = new GolemAI();
	wrapper->world = world;
	wrapper->p = p;
	wrapper->mob = mob;
	wrapper->acc = acc;
	wrapper->on_death = std::make_shared<std::function<void(HitData&)>>([entity](HitData& hit)
	{
		auto chats = entity->world->GetComponents<ChatComponent>();
		if (chats.size())
		{
			ChatMessage message;
			message.message = "A Golem has been defeated!";
			message.timeout = 10.0f;
			chats.front()->messages.push_back(message);
		}
	});
	ai->wrapper = wrapper;

	mob->on_death.add(0, wrapper->on_death);

	auto func = [=](MobComponent * target, const Vec3& v)
	{
		if (target == mob)
			return;
		target->do_damage(HitData(4, entity->get_id()));
		target->hit = true;
		target->v += v * 0.25f;
		target->takeoff_v += v * 0.2f;
	};

	auto add_hitbox = [=](const std::string& bone)
	{
		NewEntity * ent = new NewEntity();

		PositionComponent * p = new PositionComponent();
		GraphicsComponent * g = new GraphicsComponent();
		HitComponent * h = new HitComponent();

		ent->addComponent(p);
		ent->addComponent(g);
		ent->addComponent(h);

		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/sphere32_16.gmdl", Material("data/assets/white.tga"), 0)));

		h->owner = entity->get_id();
		h->bone = bone;
		h->radius = 0.2f;
		h->func = func;

		world->AddEntity(ent);
	};

	add_hitbox("Hand_R");
	add_hitbox("Hand_L");
	add_hitbox("Foot_R");
	add_hitbox("Foot_L");

	return mob;
}

#include "BlendUtility.h"

void GolemAI::tick(float dTime)
{
	auto target_entity = world->GetEntity(target);
	if (target_entity)
	{
		auto target_p = target_entity->getComponent<PositionComponent>();
		if (target_p)
		{
			Vec3 dif = target_p->p - p->p;
			float distance = dif.Len();
			Vec3 dir = dif / distance;
			if (acc->has_state("run"))
			{
				mob->facing.x = bu_angle(mob->facing.x, atan2f(-dir.x, -dir.y), exp(dTime * -5.0f));
			}
			Vec3 facing_vector = Vec3(0.0f, 0.0f, 1.0f) * Quaternion(mob->facing.y, Vec3(1.0f, 0.0f, 0.0f)) * Quaternion(-mob->facing.x, Vec3(0.0f, 0.0f, 1.0f));
			mob->move = facing_vector;
			float facing_dot_dir = facing_vector.Dot(dir);
			if (distance < 3.0f && facing_dot_dir > 0.0f  && acc->has_state("run") && delay > 0.4f)
			{
				auto stomp = new SimpleState("attack_stomp", 1.0f);
				stomp->events.insert(std::make_pair(0.7f, [=]()
				{
					auto mobs = mob->entity->world->GetNearestComponents<MobComponent>(mob->p->p, 4.0f);
					for (auto m : mobs)
					{
						if (m.second->landed && m.second != mob)
						{
							m.second->v += Vec3(0.0f, 0.0f, 10.0f);
							m.second->do_damage(HitData(4, mob->entity->get_id()));
						}
					}
				}));
				acc->set_state(stomp);
				mob->stamina.current -= 1;
				delay = 0.0f;
			}
			if (distance < 3.0f && facing_dot_dir > 0.8f && acc->has_state("run") && delay > 0.2f)
			{
				acc->set_state(new SimpleState("attack_down", 1.0f));
				mob->stamina.current -= 1;
				delay = 0.0f;
			}
			if (distance < 4.0f && facing_dot_dir > 0.5f && acc->has_state("run") && delay > 0.4f)
			{
				acc->set_state(new SimpleState("attack_sweep", 0.8f));
				mob->stamina.current -= 1;
				delay = 0.0f;
			}
			if (distance < 5.0f && facing_dot_dir > 0.5f && acc->has_state("run") && delay > 0.4f)
			{
				acc->set_state(new SimpleState("attack_sweep_low", 0.8f));
				mob->stamina.current -= 1;
				delay = 0.0f;
			}
			if (distance < 7.0f && facing_dot_dir > 0.8f && acc->has_state("run") && delay > 0.6f)
			{
				acc->set_state(new SimpleState("attack_heavy", 0.3f));
				mob->stamina.current -= 2;
				delay = 0.0f;
			}

			uint32_t mana_cost = 5;
			if (mob->mana.current >= mana_cost && facing_dot_dir > 0.8f && acc->has_state("run") && delay > 0.8f)
			{
				uint32_t power = 2;
				auto state = new SimpleState("attack_chest", 0.7f);
				state->events.insert(std::make_pair(0.5f, [=]()
				{
					mob->mana.current -= mana_cost;

					NewEntity * ent = new NewEntity();

					PositionComponent * pos = new PositionComponent();
					ProjectileComponent * projectile = new ProjectileComponent();
					GraphicsComponent * g = new GraphicsComponent(false);

					ent->addComponent(pos);
					ent->addComponent(projectile);
					ent->addComponent(g);

					projectile->v = dif + mob->v + Vec3(0.0f, 0.0f, 9.8f / 2.0f);

					pos->p = mob->p->p;

					projectile->on_collision = [=](MobComponent * target)
					{
						if (target)
						{
							if (target->temp_team != mob->temp_team)
							{
								target->do_damage(HitData(power, mob->entity->get_id()));
								target->hit = true;
							}
						}
						ent->world->SetEntity(ent->id, nullptr);

						/*auto spawn = ShadowSpawnUnit::spawn(pos->p + Vec3(0.0f, 0.0f, 1.0f), ent->world);

						spawn->v += Vec3(0.0f, 0.0f, 1.0f);

						spawn->health.current *= modifier;
						spawn->health.max *= modifier;
						spawn->stamina.current *= modifier;
						spawn->stamina.max *= modifier;
						spawn->mana.current *= modifier;
						spawn->mana.max *= modifier;

						auto spawn_acc = spawn->entity->getComponent<AnimationControlComponent>();

						spawn_acc->scale = scale;
						spawn->speed_mod = scale;
						spawn->r *= scale;*/
					};

					projectile->shooter = mob;

					g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/shadowspawn/shadowspawn.gmdl", Material("data/assets/black.tga"), 0)));
					g->decs.items.front()->local = Quaternion(-mob->facing.x, Vec3(0.0f, 0.0f, 1.0f));

					world->AddEntity(ent);
				}));
				acc->set_state(state);
				delay = 0.0f;
			}

			if (acc->has_state("run"))
				delay += dTime;
			if (distance > 60.0f)
			{
				target = EntityID();
			}
		}
	}
	else
	{
		mob->move = Vec3();
		auto nearby = world->GetNearestComponents<MobComponent>(p->p, 10.0f);
		for (auto other : nearby)
		{
			if (other.second->temp_team != mob->temp_team)
				target = other.second->entity->get_id();
		}
	}
}