#include "ShadowSpawnUnit.h"

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

class ShadowSpawnAI :
	public AIWrapper
{
public:
	void tick(float dTime);

	World * world;

	PositionComponent * p;
	MobComponent * mob;
	AnimationControlComponent * acc;

	EntityID target;
};

MobComponent * ShadowSpawnUnit::spawn(const GlobalPosition& pos, World * world)
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

	pose->anim = "data/assets/units/shadowspawn/shadowspawn.anim";
	MaterialList materials;
	materials.materials.push_back(Material("data/assets/black.tga"));
	g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/shadowspawn/shadowspawn.gmdl", materials)));

	world->AddEntity(entity);

	mob->r = 0.25f;

	mob->team = 1;

	mob->health = ResourceBar(5);
	mob->mana = ResourceBar(2);

	auto wrapper = new ShadowSpawnAI();
	wrapper->world = world;
	wrapper->p = p;
	wrapper->mob = mob;
	wrapper->acc = acc;
	ai->wrapper = wrapper;

	auto func = [=](MobComponent * target, const Vec3& v)
	{
		if (target->team == mob->team)
			return;
		target->do_damage(1, entity->get_id());
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
		h->bone = "Bone.005";
		h->radius = 0.4f;
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
		h->bone = "Bone.003";
		h->radius = 0.4f;
		h->func = func;

		world->AddEntity(ent);
	}

	return mob;
}

void ShadowSpawnAI::tick(float dTime)
{
	auto target_entity = world->GetEntity(target);
	if (target_entity)
	{
		auto target_p = target_entity->getComponent<PositionComponent>();
		auto target_mob = target_entity->getComponent<MobComponent>();
		if (target_p)
		{
			Vec3 dif = target_p->p - p->p;
			float distance = dif.Len();
			Vec3 dir = dif / distance;
			float facing_dot_dir = mob->facing.Dot(dir);
			if (distance < 1.2f * acc->scale + target_mob->r && facing_dot_dir > 0.8f && acc->has_state("run"))
			{
				acc->set_state(new SimpleState("attack", 1.0f));
			}

			if (distance > 20.0f)
			{
				target = EntityID();
			}
		}
	}
	else
	{
		mob->target_location = p->p;
		auto nearby = world->GetNearestComponents<MobComponent>(p->p, 10.0f);
		for (auto other : nearby)
		{
			if (other.second->team != mob->team)
				target = other.second->entity->get_id();
		}
	}
}