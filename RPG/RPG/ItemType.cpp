#include "ItemType.h"

std::vector<ItemType*> items;

ItemType::ItemType()
{
}

ItemType::~ItemType()
{
}

void ItemType::init()
{
	items.reserve(4096);

	items.resize(1);

	items[0] = new ItemType();
	items[0]->name = "Claymore";
	items[0]->desc = "A big sword.";
	items[0]->dec = std::make_shared<Decorator>("data/assets/items/weapons/swords/claymore.gmdl");
}

ItemType * ItemType::get(uint32_t index)
{
	return items[index];
}

void ItemType::release()
{
	for (auto item : items)
		if (item)
			delete item;
	items.clear();
}