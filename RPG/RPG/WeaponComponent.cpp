#include "WeaponComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "MobComponent.h"
#include "GraphicsComponent.h"
#include "PoseComponent.h"
#include "HitComponent.h"

#include "ClientData.h"

#include "ItemType.h"
#include "WeaponData.h"

AutoSerialFactory<WeaponComponent, Component> WeaponComponent::_factory("WeaponComponent");

WeaponComponent::WeaponComponent(void) : Component(_factory.id)
{
}

WeaponComponent::WeaponComponent(instream& is) : Component(_factory.id)
{
	is >> mob_id;
}

WeaponComponent::~WeaponComponent(void)
{
}

void WeaponComponent::connect(NewEntity * pEntity, bool authority)
{
}

void WeaponComponent::disconnect(void)
{
}

void WeaponComponent::pre_frame(float dTime)
{
}

void WeaponComponent::tick(float dTime)
{
	MobComponent * mob = nullptr;
	auto ent = entity->world->GetEntity(mob_id);
	if (ent)
		mob = ent->getComponent<MobComponent>();

	if (mob)
	{
		auto p = entity->getComponent<PositionComponent>();
		auto mob_p = mob->entity->getComponent<PositionComponent>();

		if (p && mob_p)
		{
			p->p = mob_p->p;

			if (!entity->world->authority)
			{
				auto mob_pose = mob->entity->getComponent<PoseComponent>();

				auto anim = Resource::get<SkeletalAnimation>(mob_pose->anim);
				if (anim && mob_p && mob_pose)
				{
					auto g = entity->getComponent<GraphicsComponent>();
					auto mob_g = mob->entity->getComponent<GraphicsComponent>();
					if (g && mob_g && mob_pose->pose.bones.size())
					{
						if (g->decs.items.size() && mob_g->decs.items.size() && g->decs.items.front() && mob_g->decs.items.front())
							g->decs.items.front()->local = mob_pose->pose.bones[anim->getIndex("ItemHand_R")].total_transform * mob_g->decs.items.front()->local;
					}
				}
			}
			else
				p->update();
		}
	}
}

void WeaponComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
}

void WeaponComponent::readLog(instream& is)
{
}

void WeaponComponent::writeLog(outstream& os)
{
}

void WeaponComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
}

void WeaponComponent::interpolate(Component * pComponent, float fWeight)
{
	auto other = reinterpret_cast<WeaponComponent*>(pComponent);
}

void WeaponComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
	uint32_t client_side_id = client->getUnit(mob_id.id);
	if (client_side_id != 0xffffffff)
		os << EntityID(client_side_id, client->unit_uid[client_side_id]);
	else
		os << EntityID();
}

void WeaponComponent::write_to(outstream& os) const
{
}

WeaponComponent * WeaponComponent::swap(uint32_t index)
{
	entity->world->SetEntity(entity->id, nullptr);

	NewEntity * ent = new NewEntity();

	PositionComponent * p = new PositionComponent();
	GraphicsComponent * g = new GraphicsComponent();
	WeaponComponent * w = new WeaponComponent();

	ent->addComponent(p);
	ent->addComponent(g);
	ent->addComponent(w);

	w->mob_id = mob_id;
	w->item_index = index;
	w->update();

	entity->world->AddEntity(ent);
	
	return w;
}

void WeaponComponent::update()
{
	for (auto hcid : hcids)
	{
		entity->world->SetEntity(hcid.id, nullptr);
	}
	hcids.clear();

	auto func = [=](MobComponent * target, const Vec3& v)
	{
		if (target->entity->get_id() == mob_id)
			return;
		target->do_damage(HitData(2, entity->get_id()));
		target->hit = true;
	};

	auto g = entity->getComponent<GraphicsComponent>();
	g->decs.remove(0);

	auto item = ItemType::get(item_index);
	if (item)
	{
		g->decs.add(item->dec);
		g->decs.update(0);

		auto wd = item->weapon;
		if (wd)
		{
			for (auto i : wd->hitboxes)
			{
				NewEntity * hit_ent = new NewEntity();

				PositionComponent * p = new PositionComponent();
				GraphicsComponent * g = new GraphicsComponent();
				HitComponent * h = new HitComponent();

				hit_ent->addComponent(p);
				hit_ent->addComponent(g);
				hit_ent->addComponent(h);

				g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/sphere32_16.gmdl", Material("data/assets/white.tga"), 0)));

				h->owner = mob_id;
				h->bone = i.bone;
				h->offset = i.offset;
				h->radius = i.radius;
				h->func = func;

				entity->world->AddEntity(hit_ent);

				hcids.push_back(hit_ent->get_id());
			}
		}
	}

	if (hcids.empty())
	{
		NewEntity * hit_ent = new NewEntity();

		PositionComponent * p = new PositionComponent();
		GraphicsComponent * g = new GraphicsComponent();
		HitComponent * h = new HitComponent();

		hit_ent->addComponent(p);
		hit_ent->addComponent(g);
		hit_ent->addComponent(h);

		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/sphere32_16.gmdl", Material("data/assets/white.tga"), 0)));

		h->owner = mob_id;
		h->bone = "ItemHand_R";
		h->offset = Vec3();
		h->radius = 0.1f;
		h->func = func;

		entity->world->AddEntity(hit_ent);

		hcids.push_back(hit_ent->get_id());
	}
}