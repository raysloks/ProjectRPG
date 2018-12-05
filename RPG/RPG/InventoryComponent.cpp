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
	name = "Oogabooga";
}

InventoryComponent::InventoryComponent(instream& is, bool full) : Serializable(_factory.id)
{
	is >> owner >> name;
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
							auto sprite = Resource::get<Texture>("data/assets/white.tga");

							// health
							rs.pushTransform();
							rs.addTransform(Matrix4::Translation(Vec2(0.0f, rs.size.y)));
							rs.addTransform(Matrix4::Translation(Vec3(10.0f, -10.0f, 0.0f)));
							Writing::setSize(25);
							Writing::setColor(0.75f, 0.25f, 0.25f);
							Writing::render(std::to_string((int)std::floorf(mob->health.current)) + " / " + std::to_string((int)std::floorf(mob->health.max)), rs);
							rs.popTransform();
							rs.popTransform();

							rs.pushTransform();
							rs.addTransform(Matrix4::Scale(Vec3(16.0f, mob->health.max * 4.0f, 0.0f)));
							if (sprite)
								sprite->render(rs);
							rs.popTransform();

							// stamina
							rs.pushTransform();
							rs.addTransform(Matrix4::Translation(Vec2(rs.size.x * 0.5f, rs.size.y)));
							rs.addTransform(Matrix4::Translation(Vec3(0.0f, -10.0f, 0.0f)));
							Writing::setSize(25);
							Writing::setColor(0.25f, 0.75f, 0.25f);
							Writing::setOffset(Vec2(-0.5f, 0.0f));
							Writing::render(std::to_string((int)std::floorf(mob->stamina.current)) + " / " + std::to_string((int)std::floorf(mob->stamina.max)), rs);
							Writing::setOffset(Vec2(0.0f, 0.0f));
							rs.popTransform();
							rs.popTransform();

							rs.pushTransform();
							rs.addTransform(Matrix4::Translation(Vec2(16.0f, 0.0f)));
							rs.addTransform(Matrix4::Scale(Vec3(16.0f, mob->stamina.max * 4.0f, 0.0f)));
							if (sprite)
								sprite->render(rs);
							rs.popTransform();

							// mana
							rs.pushTransform();
							rs.addTransform(Matrix4::Translation(Vec2(rs.size.x, rs.size.y)));
							rs.addTransform(Matrix4::Translation(Vec3(-10.0f, -10.0f, 0.0f)));
							Writing::setSize(25);
							Writing::setColor(0.25f, 0.25f, 0.75f);
							Writing::setOffset(Vec2(-1.0f, 0.0f));
							Writing::render(std::to_string((int)std::floorf(mob->mana.current)) + " / " + std::to_string((int)std::floorf(mob->mana.max)), rs);
							Writing::setOffset(Vec2());
							rs.popTransform();
							rs.popTransform();

							rs.pushTransform();
							rs.addTransform(Matrix4::Translation(Vec2(32.0f, 0.0f)));
							rs.addTransform(Matrix4::Scale(Vec3(16.0f, mob->mana.max * 4.0f, 0.0f)));
							if (sprite)
								sprite->render(rs);
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
								GlobalPosition text_position = p->p + Vec3(0.0f, 0.0f, 1.75f);
								Vec3 screen_pos = Vec3(text_position - entity->world->cam_pos) * rs.view;
								if (screen_pos.z < 1.0f)
								{
									rs.addTransform(Matrix4::Translation(screen_pos * Vec3(1.0f, -1.0f, 0.0f) * rs.size * 0.5f));
									float scale = rs.size.y * (1.0f / 256.0f) / Vec3(text_position - entity->world->cam_pos).Len();
									rs.addTransform(Matrix4::Scale(Vec3(scale, scale, scale)));
									rs.addTransform(Matrix4::Translation(Vec3(0.0f, 0.0f, 0.0f)));

									Writing::setOffset(Vec2(-0.5f, 0.0f));
									Writing::setSize(25);
									Writing::setColor(0.0f, 0.0f, 0.0f);
									Writing::render(name, rs);
									rs.popTransform();
									rs.addTransform(Matrix4::Translation(Vec3(-2.0f, -2.0f, 0.0f)));
									Writing::setColor(0.3f, 0.5f, 0.1f);
									Writing::render(name, rs);
									Writing::setOffset(Vec2());
									rs.popTransform();
								}
							}
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
	os << (client.client_id == client_id) << name;
}

void InventoryComponent::write_to(outstream& os) const
{
}

bool InventoryComponent::visible(ClientData& client) const
{
	return true;
	//return client.client_id == client_id;
}