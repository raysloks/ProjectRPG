#include "SpawnComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "PositionComponent.h"
#include "MobComponent.h"
#include "ColliderComponent.h"
#include "GraphicsComponent.h"
#include "AIComponent.h"
#include "AnimationControlComponent.h"
#include "PoseComponent.h"

const AutoSerialFactory<SpawnComponent> SpawnComponent::_factory("SpawnComponent");

SpawnComponent::SpawnComponent(void) : Serializable(_factory.id)
{
}

SpawnComponent::SpawnComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

SpawnComponent::~SpawnComponent(void)
{
}

void SpawnComponent::connect(NewEntity * pEntity, bool authority)
{
}

void SpawnComponent::disconnect(void)
{
}

void SpawnComponent::pre_frame(float dTime)
{
}

void SpawnComponent::post_frame(float dTime)
{
}

void SpawnComponent::tick(float dTime)
{
}

void SpawnComponent::writeLog(outstream& os, ClientData& client)
{
}

void SpawnComponent::readLog(instream& is)
{
}

void SpawnComponent::writeLog(outstream& os)
{
}

void SpawnComponent::readLog(instream& is, ClientData& client)
{
}

void SpawnComponent::interpolate(Component * pComponent, float fWeight)
{
}

void SpawnComponent::write_to(outstream& os, ClientData& client) const
{
}

void SpawnComponent::write_to(outstream& os) const
{
}

MobComponent * SpawnComponent::spawn(void)
{
	auto pos = select_position();
	if (is_valid(pos))
	{
		if (!in_view(pos, entity->world, [](MobComponent * mob)
		{
			return mob->temp_team != 1;
		}))
		{
			NewEntity * ent = new NewEntity();

			PositionComponent * p = new PositionComponent();
			p->p = pos;
			GraphicsComponent * g = new GraphicsComponent();
			AIComponent * ai = new AIComponent();
			ai->random.seed(random());
			MobComponent * mob = new MobComponent();
			AnimationControlComponent * acc = new AnimationControlComponent();
			PoseComponent * pose = new PoseComponent();

			ent->addComponent(p);
			ent->addComponent(g);
			ent->addComponent(ai);
			ent->addComponent(mob);
			ent->addComponent(acc);
			ent->addComponent(pose);

			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/units/player/KnightGuy.gmdl", Material("data/assets/terrain/textures/ngrass.tga"), 0)));
			/*g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/eyes/left.gmdl", Material("data/assets/decorators/eyes/basic.tga"))));
			g->decs.items.back()->priority = 1;
			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/eyes/right.gmdl", Material("data/assets/decorators/eyes/basic.tga"))));
			g->decs.items.back()->priority = 2;
			g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/decorators/mouth/mouth.gmdl", Material("data/assets/decorators/mouth/neutral.tga"))));
			g->decs.items.back()->priority = 3;*/

			entity->world->AddEntity(ent);

			std::uniform_real_distribution<float> uni_dist;
			std::uniform_real_distribution<float> angle_dist(0.0f, M_PI * 2.0f);
			mob->p = &p->p;
			float angle = angle_dist(random);
			mob->cam_facing = Vec3(cosf(angle), sinf(angle), 0.0f);
			mob->temp_team = 1;

			return mob;
		}
	}
	return nullptr;
}

MobComponent * SpawnComponent::spawn(const Vec3& v)
{
	auto mob = spawn();
	if (mob)
	{
		auto ai = mob->entity->getComponent<AIComponent>();
		if (ai)
		{
			std::uniform_real_distribution<float> uni_dist;
			mob->move = v;
			mob->cam_facing = v.Normalized();
			ai->checks.insert(std::make_pair(2.0f + uni_dist(random) * 6.0f, ai->chase));
		}
	}
	return mob;
}

GlobalPosition SpawnComponent::select_position(void)
{
	Vec3 dif = aabb_max - aabb_min;
	std::uniform_real_distribution<float> uni_dist;
	return aabb_min + dif * Vec3(uni_dist(random), uni_dist(random), uni_dist(random));
}

bool SpawnComponent::is_valid(const GlobalPosition& p) const
{
	return true;
}

bool SpawnComponent::inside_area(const GlobalPosition& p) const
{
	Vec3 dif_min = p - aabb_min;
	Vec3 dif_max = p - aabb_max;
	return dif_min > Vec3() && dif_max < Vec3();
}

bool SpawnComponent::in_view(const GlobalPosition& p, World * world, const std::function<bool(MobComponent*)>& pred)
{
	auto mobs = world->GetNearestComponents<MobComponent>(p);
	bool seen = false;
	for each (auto mob in mobs)
	{
		if (pred(mob.second))
		{
			std::vector<std::shared_ptr<Collision>> list;
			auto pc = mob.second->entity->getComponent<PositionComponent>();
			ColliderComponent::LineCheck(p, pc->p, list);
			if (list.empty())
			{
				seen = true;
			}
		}
	}
	return seen;
}
