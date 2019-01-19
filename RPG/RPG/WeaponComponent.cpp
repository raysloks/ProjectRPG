#include "WeaponComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "MobComponent.h"
#include "GraphicsComponent.h"
#include "PoseComponent.h"
#include "HitComponent.h"

#include "ClientData.h"

#include "WeaponData.h"

AutoSerialFactory<WeaponComponent, Component> WeaponComponent::_factory("WeaponComponent");

WeaponComponent::WeaponComponent(void) : Component(_factory.id)
{
	wd.reset(new WeaponData());

	for (size_t i = 0; i < 3; ++i)
	{
		HitboxData hbd;
		hbd.bone = "ItemHand_R";
		hbd.offset = Vec3(0.0f, 0.5f + 0.25f * i, 0.0f);
		hbd.radius = 0.2f;

		wd->hitboxes.push_back(hbd);
	}
}

WeaponComponent::WeaponComponent(instream& is, bool full) : Component(_factory.id)
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
		auto mob_pose = mob->entity->getComponent<PoseComponent>();

		auto anim = Resource::get<SkeletalAnimation>(mob_pose->anim);
		if (anim && mob_p && mob_pose)
		{
			p->p = mob_p->p;
			auto g = entity->getComponent<GraphicsComponent>();
			auto mob_g = mob->entity->getComponent<GraphicsComponent>();
			if (g && mob_g && mob_pose->pose.bones.size())
			{
				if (g->decs.items.size() && mob_g->decs.items.size())
					g->decs.items.front()->final = mob_pose->pose.bones[anim->getIndex("ItemHand_R")].total_transform * mob_g->decs.items.front()->local;
			}
		}

		if (entity->world->authority)
			p->update();
	}
}

void WeaponComponent::writeLog(outstream& os, ClientData& client)
{
}

void WeaponComponent::readLog(instream& is)
{
}

void WeaponComponent::writeLog(outstream& os)
{
}

void WeaponComponent::readLog(instream& is, ClientData& client)
{
}

void WeaponComponent::interpolate(Component * pComponent, float fWeight)
{
	auto other = reinterpret_cast<WeaponComponent*>(pComponent);
}

void WeaponComponent::write_to(outstream& os, ClientData& client) const
{
	uint32_t client_side_id = client.getUnit(mob_id.id);
	if (client_side_id != 0xffffffff)
		os << EntityID(client_side_id, client.unit_uid[client_side_id]);
	else
		os << EntityID();
}

void WeaponComponent::write_to(outstream& os) const
{
}

WeaponComponent * WeaponComponent::swap(size_t index)
{
	entity->world->SetEntity(entity->id, nullptr);

	NewEntity * ent = new NewEntity();

	PositionComponent * p = new PositionComponent();
	GraphicsComponent * g = new GraphicsComponent();
	WeaponComponent * w = new WeaponComponent();

	ent->addComponent(p);
	ent->addComponent(g);
	ent->addComponent(w);

	switch (index)
	{
	case 0:
		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/items/weapons/swords/claymore.gmdl", Material("data/assets/items/weapons/swords/claymore.tga"), 0)));
		g->decs.items.front()->local *= Quaternion(-M_PI / 2.0f, Vec3(0.0f, 1.0f, 0.0f));
		g->decs.items.front()->local *= 0.5f;
		g->decs.items.front()->local.mtrx[3][3] *= 2.0f;
		break;
	case 1:
		g->decs.add(std::shared_ptr<Decorator>(new Decorator("data/assets/medkit.gmdl", Material("data/assets/medkit.tga"), 0)));
		g->decs.items.front()->local *= Quaternion(M_PI, Vec3(0.0f, 1.0f, 0.0f)) * Quaternion(-0.25f, Vec3(1.0f, 0.0f, 0.0f));
		break;
	}
	g->decs.update(0);

	w->mob_id = mob_id;

	entity->world->AddEntity(ent);
	
	return w;
}

void WeaponComponent::updateHitbox()
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