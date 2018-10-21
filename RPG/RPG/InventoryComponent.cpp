#include "InventoryComponent.h"

#include "World.h"

#include "Client.h"
#include "ClientData.h"

#include "RenderSetup.h"

#include "Writing.h"

#include "Item.h"

#include "MobComponent.h"
#include "InteractComponent.h"

#include "Decorator.h"

const AutoSerialFactory<InventoryComponent> InventoryComponent::_factory("InventoryComponent");

InventoryComponent::InventoryComponent(void) : Serializable(_factory.id)
{
}

InventoryComponent::InventoryComponent(instream& is, bool full) : Serializable(_factory.id)
{
	is >> owner;
}

InventoryComponent::~InventoryComponent(void)
{
	if (func)
		set_display(false);
}

void InventoryComponent::connect(NewEntity * pEntity, bool authority)
{
}

void InventoryComponent::disconnect(void)
{
}

void InventoryComponent::pre_frame(float dTime)
{
	set_display(true);
	rot += dTime;
}

void InventoryComponent::tick(float dTime)
{
}

void InventoryComponent::set_display(bool enable)
{
	Client * client = entity->world->client;
	if (client != nullptr)
	{
		if (enable)
		{
			if (!func)
			{
				auto mob = entity->getComponent<MobComponent>();
				if (mob != nullptr)
				{
					if (entity->world->authority)
						owner = entity->world->client->clientData->client_id == client_id;
					if (owner)
					{
						func.reset(new std::function<void(RenderSetup&)>([this, mob](RenderSetup& rs)
						{

							// health
							rs.pushTransform();
							rs.addTransform(Matrix4::Translation(Vec3(100.0f, 100.0f, 0.0f)));
							Writing::setSize(25);
							Writing::setColor(1.0f, 0.0f, 0.0f);
							Writing::render(std::to_string((int)std::floorf(mob->health.current)) + " / " + std::to_string((int)std::floorf(mob->health.max)), rs);
							rs.popTransform();
							rs.popTransform();

							// stamina
							rs.pushTransform();
							rs.addTransform(Matrix4::Translation(Vec3(100.0f, 140.0f, 0.0f)));
							Writing::setSize(25);
							Writing::setColor(0.0f, 1.0f, 0.0f);
							Writing::render(std::to_string((int)std::floorf(mob->stamina.current)) + " / " + std::to_string((int)std::floorf(mob->stamina.max)), rs);
							rs.popTransform();
							rs.popTransform();

							// mana
							rs.pushTransform();
							rs.addTransform(Matrix4::Translation(Vec3(100.0f, 180.0f, 0.0f)));
							Writing::setSize(25);
							Writing::setColor(0.0f, 0.0f, 1.0f);
							Writing::render(std::to_string((int)std::floorf(mob->mana.current)) + " / " + std::to_string((int)std::floorf(mob->mana.max)), rs);
							rs.popTransform();
							rs.popTransform();
						}));
					}
					else
					{
						func.reset(new std::function<void(RenderSetup&)>([this, mob](RenderSetup& rs) {
							rs.pushTransform();
							PositionComponent * p = entity->getComponent<PositionComponent>();
							if (p)
							{
								rs.addTransform(Matrix4::Translation(Vec3(rs.size * 0.5f)));
								rs.addTransform(Matrix4::Translation(Vec3(Vec3(p->p - entity->world->cam_pos) * rs.view * Vec3(1.0f, -1.0f, 0.0f) * rs.size * 0.5f)));
								float scale = 4.0f / Vec3(p->p - entity->world->cam_pos).Len();
								rs.addTransform(Matrix4::Scale(Vec3(scale, scale, scale)));
								rs.addTransform(Matrix4::Translation(Vec3(0.0f, -64.0f, 0.0f)));
							}
							Writing::setOffset(Vec2(-0.5f, 0.0f));
							Writing::setSize(25);
							Writing::setColor(0.3f, 0.5f, 0.1f);
							Writing::render("Ally guy test", rs);
							Writing::setOffset(Vec2());
							rs.popTransform();
							rs.popTransform();
						}));
					}
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
	os << (client.client_id == client_id);
}

void InventoryComponent::write_to(outstream& os) const
{
}

bool InventoryComponent::visible(ClientData& client) const
{
	return true;
	//return client.client_id == client_id;
}