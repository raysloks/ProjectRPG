#include "InventoryComponent.h"

#include "World.h"

#include "Client.h"
#include "ClientData.h"

#include "RenderSetup.h"

#include "Writing.h"

#include "Item.h"

#include "MobComponent.h"

const AutoSerialFactory<InventoryComponent> InventoryComponent::_factory("InventoryComponent");

InventoryComponent::InventoryComponent(void) : Serializable(_factory.id)
{
}

InventoryComponent::InventoryComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

InventoryComponent::~InventoryComponent(void)
{
	if (func)
		set_display(false);
}

void InventoryComponent::connect(NewEntity * pEntity, bool authority)
{
	items.setSyncState(&pEntity->ss);
	std::shared_ptr<Item> item(new Item());
	item->type = 100;
	items.add(item);
}

void InventoryComponent::disconnect(void)
{
}

void InventoryComponent::pre_frame(float dTime)
{
	set_display(true);
}

void InventoryComponent::tick(float dTime)
{
}

void InventoryComponent::set_display(bool enable)
{
	Client * client = entity->world->client;
	if (client != nullptr)
	{
		if (client->clientData != nullptr)
			enable &= visible(*client->clientData);
		if (enable)
		{
			if (!func)
			{
				auto mob = entity->getComponent<MobComponent>();
				if (mob != nullptr)
				{
					func.reset(new std::function<void(RenderSetup&)>([this, mob](RenderSetup& rs) {
						rs.pushTransform();
						rs.addTransform(Matrix4::Translation(Vec3(100.0f, 100.0f, 0.0f)));
						Writing::setSize(25);
						Writing::render(std::to_string((int)std::ceilf(mob->health.current)) + " / " + std::to_string((int)std::ceilf(mob->health.max)), rs);
						rs.popTransform();
						rs.popTransform();
						/*for (auto i = items.items.begin(); i != items.items.end(); ++i)
						{
							Writing::render(std::to_string((*i)->type), rs);
						}*/
						rs.pushTransform();
						rs.addTransform(Matrix4::Translation(Vec3(960.0f, 540.0f, 0.0f)));
						rs.addTransform(Matrix4::Translation(Vec3(-10.0f, 10.0f, 0.0f)));
						Writing::render("O", rs);
						rs.popTransform();
						rs.popTransform();
					}));
					client->render2D.push_back(func);
				}
			}
		}
		else
		{
			if (func)
			{
				client->render2D.erase(std::remove(client->render2D.begin(), client->render2D.end(), func), client->render2D.end());
				func.reset();
			}
		}
	}
}

void InventoryComponent::writeLog(outstream& os, ClientData& client)
{
}

void InventoryComponent::readLog(instream& is)
{
}

void InventoryComponent::writeLog(outstream& os)
{
}

void InventoryComponent::readLog(instream& is, ClientData& client)
{
}

void InventoryComponent::interpolate(Component * pComponent, float fWeight)
{
}

void InventoryComponent::write_to(outstream& os, ClientData& client) const
{
}

void InventoryComponent::write_to(outstream& os) const
{
}

bool InventoryComponent::visible(ClientData& client) const
{
	return true;
	//return client.unit_id == entity->id;
}