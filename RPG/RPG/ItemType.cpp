#include "ItemType.h"

#include "WeaponData.h"

std::vector<ItemType> items;

ItemType::ItemType()
{
}

ItemType::~ItemType()
{
	if (weapon)
		delete weapon;
}

void ItemType::init()
{
	items.reserve(4096);

	items.resize(3);

	items[0].name = "Claymore";
	items[0].desc = "A big sword.";
	items[0].dec = std::make_shared<Decorator>("data/assets/items/weapons/swords/claymore.gmdl");
	items[0].dec->bone_id = 0;

	items[0].weapon = new WeaponData();
	for (size_t i = 0; i < 3; ++i)
	{
		HitboxData hbd;
		hbd.bone = "ItemHand_R";
		hbd.offset = Vec3(0.0f, 0.5f + 0.25f * i, 0.0f);
		hbd.radius = 0.2f;

		items[0].weapon->hitboxes.push_back(hbd);
	}

	items[1].name = "Sword";
	items[1].desc = "A big sword.";
	items[1].dec = std::make_shared<Decorator>("data/assets/items/weapons/swords/sword.gmdl");
	items[1].dec->bone_id = 0;

	items[1].weapon = new WeaponData();
	for (size_t i = 0; i < 3; ++i)
	{
		HitboxData hbd;
		hbd.bone = "ItemHand_R";
		hbd.offset = Vec3(0.0f, 0.3f + 0.25f * i, 0.0f);
		hbd.radius = 0.2f;

		items[1].weapon->hitboxes.push_back(hbd);
	}

	items[2].name = "Useless Sword";
	items[2].desc = "A useless sword.";
	items[2].dec = std::make_shared<Decorator>("data/assets/items/weapons/swords/sword.gmdl");
	items[2].dec->bone_id = 0;
}

const ItemType * ItemType::get(uint32_t index)
{
	if (index < items.size())
		return &items[index];
	return nullptr;
}

void ItemType::release()
{
	items.clear();
}