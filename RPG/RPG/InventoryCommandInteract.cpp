#include "InventoryCommandInteract.h"

#include "NewEntity.h"

#include "World.h"

#include "InventoryComponent.h"
#include "MobComponent.h"
#include "InteractComponent.h"

AutoSerialFactory<InventoryCommandInteract, InventoryCommand> InventoryCommandInteract::_factory("InventoryCommandInteract");

InventoryCommandInteract::InventoryCommandInteract(const EntityID& id)
{
	entity_id = id;
}

InventoryCommandInteract::InventoryCommandInteract(instream& is, bool full)
{
	is >> entity_id;
}

InventoryCommandInteract::~InventoryCommandInteract()
{
}

void InventoryCommandInteract::execute(InventoryComponent& inv)
{
	auto mob = inv.entity->getComponent<MobComponent>();
	if (mob)
	{
		auto ent = inv.entity->world->GetEntity(entity_id);
		if (ent)
		{
			auto p = ent->getComponent<PositionComponent>();
			auto ic = ent->getComponent<InteractComponent>();
			if (p && ic)
			{
				if (Vec3(p->p - mob->p->p).LenPwr() <= 2.3f * 2.3f)
				{
					if (ic->func)
					{
						ic->func(mob);
					}
				}
			}
		}
	}
}

void InventoryCommandInteract::write_to(outstream& os) const
{
	os << entity_id;
}

uint32_t InventoryCommandInteract::getSerial() const
{
	return _factory.id;
}