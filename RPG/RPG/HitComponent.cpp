#include "HitComponent.h"

#include "NewEntity.h"
#include "World.h"

#include "Wall.h"

#include "PositionComponent.h"
#include "GraphicsComponent.h"
#include "PoseComponent.h"
#include "MobComponent.h"
#include "AnimationControlComponent.h"

const AutoSerialFactory<HitComponent> HitComponent::_factory("HitComponent");

HitComponent::HitComponent(void) : Serializable(_factory.id)
{
}

HitComponent::HitComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

HitComponent::~HitComponent(void)
{
}

void HitComponent::connect(NewEntity * pEntity, bool authority)
{
}

void HitComponent::disconnect(void)
{
}

void HitComponent::pre_frame(float dTime)
{
}

void HitComponent::tick(float dTime)
{
	if (!entity->world->authority)
		return;

	auto p = entity->getComponent<PositionComponent>();
	auto g = entity->getComponent<GraphicsComponent>();

	float r = 0.0f;
	active = false;
	auto ent = entity->world->GetEntity(owner);
	if (ent)
	{
		auto pose = ent->getComponent<PoseComponent>();
		auto anim = Resource::get<SkeletalAnimation>(pose->anim);
		if (anim)
		{
			auto mob = ent->getComponent<MobComponent>();
			auto acc = ent->getComponent<AnimationControlComponent>();

			float prop = anim->getProperty(bone + ".active", pose->frame);
			GlobalPosition pos = Vec3() * anim->getMatrix(anim->getIndex(bone), pose->frame) * acc->transform;
			r = radius * acc->scale * prop;
			p->p = pos;
			p->update();
			if (g)
			{
				g->decs.items.front()->local = Matrix4::Scale(Vec3(r, r, r));
				g->decs.update(0);
			}
			active = prop > 0.5f;
		}
	}
	else
	{
		entity->world->SetEntity(entity->id, nullptr);
	}

	std::set<MobComponent*> next_hit;

	if (active)
	{
		for (auto ent : entity->world->units)
		{
			if (ent)
			{
				MobComponent * mob = ent->getComponent<MobComponent>();
				if (mob)
				{
					auto mob_p = ent->getComponent<PositionComponent>();
					if (mob_p)
					{
						GlobalPosition target = mob_p->p;
						GlobalPosition prev_target = target;
						float r_plus_r = r + 0.5f;

						auto c = Wall::SphereLine(prev_p - prev_target, p->p - target, Vec3(), r_plus_r);

						if (c || Vec3(prev_p - prev_target).LenPwr() <= r_plus_r * r_plus_r)
						{
							next_hit.insert(mob);
							if (hit.find(mob) == hit.end())
							{
								if (func)
									func(mob, Vec3(p->p - prev_p) / dTime);
							}
						}
					}
				}
			}
		}
	}

	hit = next_hit;
	prev_p = p->p;
	prev_r = r;
}

void HitComponent::writeLog(outstream& os, ClientData& client)
{
}

void HitComponent::readLog(instream& is)
{
}

void HitComponent::writeLog(outstream& os)
{
}

void HitComponent::readLog(instream& is, ClientData& client)
{
}

void HitComponent::interpolate(Component * pComponent, float fWeight)
{
}

void HitComponent::write_to(outstream& os, ClientData& client) const
{
}

void HitComponent::write_to(outstream& os) const
{
}