#include "InventoryComponent.h"

#include "World.h"

#include "Client.h"
#include "ClientData.h"

#include "RenderSetup.h"

#include "Writing.h"

#include "Item.h"

const AutoSerialFactory<InventoryComponent> InventoryComponent::_factory("InventoryComponent");

InventoryComponent::InventoryComponent(void) : Serializable(_factory.id)
{
}

InventoryComponent::InventoryComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

InventoryComponent::~InventoryComponent(void)
{
	if (func != 0)
		set_display(false);
}

void InventoryComponent::connect(NewEntity * pEntity, bool authority)
{
	items.setSyncState(&pEntity->ss);
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
		enable = false;
		if (client->clientData != nullptr)
			enable &= visible(*client->clientData);
		if (enable)
		{
			if (func==0)
			{
				func.reset(new std::function<void(RenderSetup&)>([this](RenderSetup& rs) {
					glBegin(GL_QUADS);

					glColor4f(0.0f, 0.0f, 0.0f, 0.85f);

					glVertex2f(rs.size.x*0.1f, rs.size.y*0.1f);
					glVertex2f(rs.size.x*0.9f, rs.size.y*0.1f);

					glColor4f(0.0f, 0.0f, 0.0f, 0.0f);

					glVertex2f(rs.size.x*0.9f, rs.size.y);
					glVertex2f(rs.size.x*0.1f, rs.size.y);

					glEnd();

					glPushMatrix();
					glOrtho(0.0, rs.size.x, rs.size.y, 0.0, 0.0, 0.0);
					glTranslatef(rs.size.x * 0.1 + 60.0f, rs.size.y * 0.1 + 60.0f, 0.0f);
					for (auto i = items.items.begin(); i != items.items.end(); ++i)
					{
						Writing::render(std::to_string((*i)->type));
						glTranslatef(0.0f, 16.0f, 0.0f);
					}
					glPopMatrix();
				}));
				client->render2D.push_back(func);
			}
		}
		else
		{
			if (func!=0)
			{
				client->render2D.erase(std::remove(client->render2D.begin(), client->render2D.end(), func), client->render2D.end());
				func = 0;
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