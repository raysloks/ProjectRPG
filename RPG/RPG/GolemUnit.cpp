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
	materials.materials.back().tex.push_back("data/assets/units/golem/golem_ao.tga");
	g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/golem/golem.gmdl", materials)));
	g->tag = 1;

	acc->scale = 3.0f;

	world->AddEntity(entity);

	mob->r = 1.0f;
	
	mob->team = 1;

	mob->health = ResourceBar(100);
	mob->mana = ResourceBar(50);

	auto wrapper = new GolemAI();
	wrapper->world = world;
	wrapper->p = p;
	wrapper->mob = mob;
	wrapper->acc = acc;
	ai->wrapper = wrapper;

	auto func = [=](MobComponent * target, const Vec3& v)
	{
		if (target == mob)
			return;
		target->do_damage(4, entity->get_id());
		target->v += v * 0.2f;
	};

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
		h->bone = "Hand_R";
		h->radius = 0.2f;
		h->func = func;

		world->AddEntity(ent);
	}

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
		h->bone = "Hand_L";
		h->radius = 0.2f;
		h->func = func;

		world->AddEntity(ent);
	}

	return mob;
}

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
			float facing_dot_dir = mob->facing.Dot(dir);
			if (distance < 3.0f && facing_dot_dir > 0.8f && acc->has_state("run") && delay > 0.2f)
			{
				acc->set_state(new SimpleState("attack_down", 1.0f));
				delay = 0.0f;
			}
			if (distance < 5.0f && facing_dot_dir > 0.8f && acc->has_state("run") && delay > 0.4f)
			{
				acc->set_state(new SimpleState("attack_sweep", 0.8f));
				delay = 0.0f;
			}
			if (distance < 7.0f && facing_dot_dir > 0.8f && acc->has_state("run") && delay > 0.6f)
			{
				acc->set_state(new SimpleState("attack_heavy", 0.3f));
				delay = 0.0f;
			}
			if (mob->mana.current > 0 && facing_dot_dir > 0.8f && acc->has_state("run") && delay > 0.8f)
			{
				uint32_t power = std::min(5, mob->mana.current);
				float modifier = power / 5.0f;
				float scale = cbrtf(modifier);
				auto state = new SimpleState("attack_chest", 0.7f);
				state->events.insert(std::make_pair(0.5f, [=]()
				{
					mob->mana.current -= power;

					NewEntity * ent = new NewEntity();

					PositionComponent * pos = new PositionComponent();
					ProjectileComponent * projectile = new ProjectileComponent();
					GraphicsComponent * g = new GraphicsComponent(false);

					ent->addComponent(pos);
					ent->addComponent(projectile);
					ent->addComponent(g);

					projectile->v = dif + mob->v + Vec3(0.0f, 0.0f, 9.8f / 2.0f);

					pos->p = p->p;

					projectile->on_collision = [=](MobComponent * target)
					{
						if (target)
						{
							if (target->team != mob->team)
							{
								target->do_damage(power, mob->entity->get_id());
							}
						}
						ent->world->SetEntity(ent->id, nullptr);

						auto spawn = ShadowSpawnUnit::spawn(pos->p + Vec3(0.0f, 0.0f, 1.0f), ent->world);

						spawn->v += Vec3(0.0f, 0.0f, 1.0f);

						spawn->health.current *= modifier;
						spawn->health.max *= modifier;
						spawn->mana.current *= modifier;
						spawn->mana.max *= modifier;

						auto spawn_acc = spawn->entity->getComponent<AnimationControlComponent>();

						spawn_acc->scale = scale;
						spawn->r *= scale;
					};

					projectile->shooter = mob;

					g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/shadowspawn/shadowspawn.gmdl", Material("data/assets/black.tga"), 0)));

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
		mob->target_location = p->p;
		auto nearby = world->GetNearestComponents<MobComponent>(p->p, 30.0f);
		for (auto other : nearby)
		{
			if (other.second->team != mob->team)
				target = other.second->entity->get_id();
		}
	}
}