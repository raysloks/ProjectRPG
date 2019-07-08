#include "InventoryCommandInteract.h"

#include "NewEntity.h"

#include "World.h"

#include "InventoryComponent.h"
#include "MobComponent.h"
#include "InteractComponent.h"

#include "ClientData.h"

#include "ChatComponent.h"

AutoSerialFactory<InventoryCommandInteract, InventoryCommand> InventoryCommandInteract::_factory("InventoryCommandInteract");

InventoryCommandInteract::InventoryCommandInteract(const EntityID& id)
{
	entity_id = id;
}

InventoryCommandInteract::InventoryCommandInteract(instream& is)
{
	is >> entity_id;
}

InventoryCommandInteract::~InventoryCommandInteract()
{
}

void InventoryCommandInteract::execute(InventoryComponent& inv)
{
	auto client = inv.clientData.lock();

	entity_id = client->getRealID(entity_id);
	if (entity_id.id < inv.entity->world->uid.size())
		entity_id.uid = inv.entity->world->uid[entity_id.id];

	auto mob = inv.entity->getComponent<MobComponent>();
	if (mob)
	{
		/*ChatMessage message;
		message.message = std::to_string(entity_id.id) + " " + std::to_string(entity_id.uid);
		message.timeout = 10.0f;
		inv.entity->world->GetComponents<ChatComponent>().front()->messages.push_back(message);*/
		auto ent = inv.entity->world->GetEntity(entity_id);
		if (ent)
		{
			auto p = ent->getComponent<PositionComponent>();
			auto ic = ent->getComponent<InteractComponent>();
			if (p && ic)
			{
				if (Vec3(p->p - mob->p->p).LenPwr() <= 2.0f * 2.0f)
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