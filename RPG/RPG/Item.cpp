#include "Item.h"

#include "World.h"

#include "ItemType.h"

#include "NewEntity.h"

#include "GraphicsComponent.h"
#include "InteractComponent.h"
#include "MobComponent.h"
#include "InventoryComponent.h"

Item::Item()
{
	type = 0;
}

Item::Item(uint32_t t)
{
	type = t;
}

Item::~Item()
{
}

void Item::writeLog(outstream& os) const
{
	os << type;
}

void Item::readLog(instream& is)
{
	is >> type;
}

NewEntity * Item::drop(World * world, uint32_t type, const GlobalPosition& pos)
{
	ItemType * item_type = ItemType::get(type);

	if (item_type)
	{
		NewEntity * ent = new NewEntity();

		PositionComponent * p = new PositionComponent(pos);
		GraphicsComponent * g = new GraphicsComponent(false);
		InteractComponent * i = new InteractComponent();

		ent->addComponent(p);
		ent->addComponent(g);
		ent->addComponent(i);

		g->decs.add(item_type->dec);

		i->name = item_type->name;
		i->action_name = "Pick Up";

		i->func = [=](MobComponent * user)
		{
			auto inv = user->entity->getComponent<InventoryComponent>();
			if (inv)
			{
				auto item = std::make_shared<Item>(type);
				inv->items.add(item);
				inv->notifications.queue.push_back(*item);
				world->SetEntity(ent->id, nullptr);
			}
		};

		world->AddEntity(ent);

		return ent;
	}

	return nullptr;
}

outstream& operator<<(outstream& os, const Item& item)
{
	item.writeLog(os);
	return os;
}

instream& operator>>(instream& is, Item& item)
{
	item.readLog(is);
	return is;
}