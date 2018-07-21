#include "Item.h"

Item::Item()
{
	name = "Shadowmourne";
	dec.reset(new Decorator("data/assets/items/weapons/swords/claymore.gmdl", Material("data/assets/items/weapons/swords/claymore.tga"), 0));
}

Item::~Item()
{
}