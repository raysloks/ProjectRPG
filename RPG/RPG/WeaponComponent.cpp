#include "WeaponComponent.h"

#include "NewEntity.h"

#include "World.h"

#include "MobComponent.h"
#include "GraphicsComponent.h"
#include "PoseComponent.h"

#include "ClientData.h"

ASF_C(WeaponComponent, Component)

WeaponComponent::WeaponComponent(void) : Component(_factory.id)
{
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
		if (anim)
		{
			p->p = mob_p->p;
			auto g = entity->getComponent<GraphicsComponent>();
			auto mob_g = mob->entity->getComponent<GraphicsComponent>();
			if (g && mob_g)
			{
				if (g->decs.items.size() && mob_g->decs.items.size() && mob_pose->pose)
					g->decs.items.front()->final = mob_pose->pose->bones[anim->getIndex("ItemHand_R")].total_transform * mob_g->decs.items.front()->local;
			}
		}

		if (entity->world->authority)
			p->update();
	}
}

void WeaponComponent::writeLog(outstream& os, ClientData& client)
{
	os << recoil;
}

void WeaponComponent::readLog(instream& is)
{
	is >> recoil;
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
	recoil = recoil * (1.0f - fWeight) + other->recoil * fWeight;
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
