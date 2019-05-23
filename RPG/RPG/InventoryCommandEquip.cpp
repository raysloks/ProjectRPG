#include "InventoryCommandEquip.h"

#include "NewEntity.h"

#include "World.h"

#include "InventoryComponent.h"
#include "MobComponent.h"
#include "WeaponComponent.h"

#include "Item.h"

AutoSerialFactory<InventoryCommandEquip, InventoryCommand> InventoryCommandEquip::_factory("InventoryCommandEquip");

InventoryCommandEquip::InventoryCommandEquip(uint32_t i)
{
	index = i;
}

InventoryCommandEquip::InventoryCommandEquip(instream& is, bool full)
{
	is >> index;
}

InventoryCommandEquip::~InventoryCommandEquip()
{
}

void InventoryCommandEquip::execute(InventoryComponent& inv)
{
	auto mob = inv.entity->getComponent<MobComponent>();
	if (mob && mob->weapon)
	{
		auto item = inv.items.get(index);
		if (item)
		{
			mob->weapon->item_index = item->type;
			mob->weapon->update();
		}
	}
}

void InventoryCommandEquip::write_to(outstream& os) const
{
	os << index;
}

uint32_t InventoryCommandEquip::getSerial() const
{
	return _factory.id;
}