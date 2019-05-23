#include "InventoryComponent.h"

#include "World.h"

#include "Client.h"
#include "ClientData.h"

#include "RenderSetup.h"

#include "Writing.h"

#include "Item.h"
#include "ItemType.h"

#include "MobComponent.h"
#include "InteractComponent.h"
#include "WeaponComponent.h"

#include "Decorator.h"

#include "RectangleWindow.h"
#include "RectangleButtonWindow.h"
#include "TextWindow.h"

#include "InventoryCommandInteract.h"
#include "InventoryCommandEquip.h"

AutoSerialFactory<InventoryComponent, Component> InventoryComponent::_factory("InventoryComponent");

InventoryComponent::InventoryComponent(void) : Component(_factory.id)
{
	name = "Oogabooga";
}

InventoryComponent::InventoryComponent(instream& is, bool full) : Component(_factory.id)
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
	if (authority)
	{
		items.setSyncState(&pEntity->ss);
	}
}

void InventoryComponent::disconnect(void)
{
}

void InventoryComponent::pre_frame(float dTime)
{
	set_display(true);

	if (notifications.queue.size())
	{
		if (notifications_display.empty())
		{
			notifications_display.push_back(std::make_pair(0.0f, notifications.queue.front()));
			notifications.queue.erase(notifications.queue.begin());
		}
		else
		{
			float dif = notifications_display.back().first - 1.25f;
			if (dif >= 0.0f)
			{
				notifications_display.push_back(std::make_pair(dif, notifications.queue.front()));
				notifications.queue.erase(notifications.queue.begin());
			}
		}
	}

	for (auto& i : notifications_display)
	{
		i.first += dTime;
	}
	notifications_display.erase(std::remove_if(notifications_display.begin(), notifications_display.end(),
		[](const std::pair<float, Item>& i) { return i.first > 1.5f; }), notifications_display.end());

	Client * client = entity->world->client;
	if (client)
	{
		const Input& input = client->input;

		if (input.isPressed(Platform::KeyEvent::TAB))
		{
			if (window)
			{
				client->windows.erase(std::remove(client->windows.begin(), client->windows.end(), window));
				window.reset();
			}
			else
			{
				window.reset(new Window());
				client->windows.push_back(window);
				window->onRender = [=]()
				{
					client->hideCursor = false;
				};

				auto rect_window = new RectangleWindow(400.0f, 200.0f, 1000.0f, 1000.0f);
				window->addChild(rect_window);

				int i = 0;
				for (auto item : items.items)
				{
					++i;

					auto type = ItemType::get(item->type);

					auto text = new TextWindow(20, 40 * i + 16, 100, 32);
					text->text = type->name;
					auto button = new RectangleButtonWindow(120, 40 * i, 100, 32);
					button->text = "Equip";
					button->onClick = [=]()
					{
						commands.queue.emplace_back(new InventoryCommandEquip(i - 1));
						/*auto mob = entity->getComponent<MobComponent>();
						if (mob && mob->weapon)
						{
							mob->weapon->item_index = item->type;
							mob->weapon->update();
						}*/
					};
					rect_window->addChild(text);
					rect_window->addChild(button);
				}
			}
		}

		if (input.isPressed(Platform::KeyEvent::E))
		{
			commands.queue.emplace_back(new InventoryCommandInteract(current_interact));
			//interact.queue.emplace_back(current_interact);
		}
	}
}

void InventoryComponent::tick(float dTime)
{
	if (entity->world->authority)
	{
		for (auto command : commands.queue)
		{
			command.ptr->execute(*this);
		}
		commands.queue.clear();

		auto mob = entity->getComponent<MobComponent>();
		if (mob)
		{
			for (auto i : interact.queue)
			{
				auto ent = entity->world->GetEntity(i);
				if (ent)
				{
					auto p = ent->getComponent<PositionComponent>();
					auto ic = ent->getComponent<InteractComponent>();
					if (p && ic)
					{
						if (Vec3(p->p - mob->p->p).LenPwr() <= 5.29f)
						{
							if (ic->func)
							{
								ic->func(mob);
							}
						}
					}
				}
			}
			interact.queue.clear();
		}
	}
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
							Vec4 color(1.0f);

							ShaderMod mod(ShaderProgram::Get("data/gui_vert.txt", "data/gui_frag.txt"), [&color](const std::shared_ptr<ShaderProgram>& prog)
							{
								prog->Uniform("color", color);
							});

							ShaderMod mod3d(ShaderProgram::Get("data/gfill_vert.txt", "data/gfill_frag.txt"), [&color](const std::shared_ptr<ShaderProgram>& prog)
							{
								prog->Uniform("color", color);
								prog->Uniform("light", Vec3(1.0f, -1.0f, 1.0f).Normalize());
							});


							current_interact = EntityID();
							auto p = entity->getComponent<PositionComponent>();
							GlobalPosition interact_sphere_pos = p->p + Vec3(0.0f, 0.0f, 1.0f) * Quaternion(mob->facing.y, Vec3(1.0f, 0.0f, 0.0f)) * Quaternion(-mob->facing.x, Vec3(0.0f, 0.0f, 1.0f));
							auto interacts = entity->world->GetNearestComponents<InteractComponent>(interact_sphere_pos, 1.0f);
							if (!interacts.empty())
							{
								current_interact = interacts.begin()->second->entity->get_id();
								auto other_p = interacts.begin()->second->entity->getComponent<PositionComponent>();
								rs.pushTransform();
								rs.addTransform(Matrix4::Translation(Vec3(rs.size * 0.5f)));
								Vec3 world_pos = Vec3(other_p->p - entity->world->cam_pos) * 0.5f + Vec3(interact_sphere_pos - entity->world->cam_pos) * 0.5f;
								rs.addTransform(Matrix4::Translation(Vec3(world_pos * rs.view * Vec3(1.0f, -1.0f, 0.0f) * rs.size * 0.5f)));
								rs.pushTransform();
								rs.addTransform(Matrix4::Translation(Vec3(20.0f, 0.0f, 0.0f)));
								Writing::setSize(24);
								Writing::setColor(1.0f, 1.0f, 1.0f);
								Writing::render(interacts.begin()->second->action_name, rs);
								rs.popTransform();
								rs.addTransform(Matrix4::Translation(Vec3(-40.0f, -32.0f, 0.0f)));
								Writing::setSize(16);
								Writing::setColor(1.0f, 1.0f, 1.0f);
								Writing::render(interacts.begin()->second->name, rs);
								rs.popTransform();
								rs.popTransform();
								rs.addTransform(Matrix4::Translation(Vec3(-32.0f, -32.0f, 0.0f)));
								auto crosshair = Resource::get<Texture>("data/assets/interact.tga");
								if (crosshair)
									crosshair->render(rs);
								rs.popTransform();
							}


							Writing::setOffset(Vec2(-0.5f, 0.0f));
							for (auto i : notifications_display)
							{
								rs.pushTransform();
								float a = fminf(1.0f, fminf(i.first * 4.0f, 6.0f - i.first * 4.0f));
								a = sqrt(a);
								rs.addTransform(Matrix4::Translation(rs.size * 0.5f + Vec2(0.0f, rs.size.y - a * rs.size.y)));

								auto type = ItemType::get(i.second.type);
								if (type)
								{
									if (type->dec)
									{
										glEnable(GL_DEPTH_TEST);
										glDepthMask(GL_TRUE);
										rs.pushTransform();
										rs.addTransform(Quaternion(1.0f, Vec3(-1.0f, 1.0f, 0.0f).Normalize()));
										rs.addTransform(Matrix4::Scale(Vec3(256.0f)));
										rs.pushMod(mod3d);
										type->dec->render(rs);
										rs.popMod();
										rs.popTransform();
										glDepthMask(GL_FALSE);
										glDisable(GL_DEPTH_TEST);
									}

									Writing::setColor(Vec4(1.0f));
									Writing::setSize(32);
									Writing::render(type->name, rs);
									rs.popTransform();
								}

								rs.popTransform();
							}


							auto sprite = Resource::get<Texture>("data/assets/white.tga");

							float bar_width = 32.0f;
							float bar_margin = 0.0f;

							Vec2 offset[3];
							Vec2 size[3];
							Vec4 bar_color[3] = {
								Vec4(0.6f, 0.1f, 0.1f, 0.8f),
								Vec4(0.1f, 0.6f, 0.1f, 0.8f),
								Vec4(0.1f, 0.1f, 0.6f, 0.8f)
							};
							Vec2 bg_offset[3];
							Vec2 bg_size[3];
							Vec2 text_offset[3];
							Vec4 text_color[3] = {
								Vec4(1.0f, 0.9f, 0.9f, 1.0f),
								Vec4(0.9f, 1.0f, 0.9f, 1.0f),
								Vec4(0.9f, 0.9f, 1.0f, 1.0f)
							};

							for (int i = 0; i < 3; ++i)
							{
								float length_divider = sqrt(mob->resource[i].max) + 8.0f;
								length_divider *= length_divider;

								float bar_length = rs.size.x * 0.5f * mob->resource[i].max / length_divider + 64.0f;

								offset[i] = Vec2(rs.size.x * 0.5f - bar_length * 0.5f, rs.size.y * 0.75f + (bar_width + bar_margin) * i);
								size[i] = Vec2(mob->resource[i].current * bar_length / mob->resource[i].max, bar_width);
								bg_offset[i] = Vec2(rs.size.x * 0.5f - bar_length * 0.5f, rs.size.y * 0.75f + (bar_width + bar_margin) * i);
								bg_size[i] = Vec2(bar_length, bar_width);
								text_offset[i] = Vec2(rs.size.x * 0.5f, rs.size.y * 0.75f + (bar_width + bar_margin) * i + 24.0f);
							}

							float fade_multiplier = 1.0f;

							rs.pushMod(mod);

							for (int i = 0; i < 3; ++i)
							{
								color = Vec4(0.0f, 0.0f, 0.0f, 0.5f);
								if (mob->resource[i].current == mob->resource[i].max)
									color.w *= fade_multiplier;
								rs.pushTransform();
								rs.addTransform(Matrix4::Translation(bg_offset[i]));
								if (sprite)
									sprite->render(rs, bg_size[i]);
								rs.popTransform();
							}

							for (int i = 0; i < 3; ++i)
							{
								color = bar_color[i];
								if (mob->resource[i].current == mob->resource[i].max)
									color.w *= fade_multiplier;
								rs.pushTransform();
								rs.addTransform(Matrix4::Translation(offset[i]));
								if (sprite)
									sprite->render(rs, size[i]);
								rs.popTransform();
							}

							rs.popMod();
							
							int font_size = 24;
							Writing::setSize(font_size);

							for (int i = 0; i < 3; ++i)
							{
								rs.pushTransform();
								rs.addTransform(Matrix4::Translation(text_offset[i]));
								if (mob->resource[i].current == mob->resource[i].max)
									text_color[i].w *= fade_multiplier;
								Writing::setColor(text_color[i]);
								Writing::setOffset(Vec2(-0.5f, 0.0f));
								Writing::render("/", rs);
								rs.popTransform();
								Writing::setOffset(Vec2(-1.0f, 0.0f));
								Writing::render(std::to_string((int)std::floorf(mob->resource[i].current)) + "  ", rs);
								rs.popTransform();
								Writing::setOffset(Vec2(0.0f, 0.0f));
								Writing::render("  " + std::to_string((int)std::floorf(mob->resource[i].max)), rs);
								rs.popTransform();
								rs.popTransform();
							}

							Writing::setOffset(Vec2(0.0f, 0.0f));

							if (false)
							{
								if (sprite)
								{
									rs.pushTransform();
									rs.addTransform(Matrix4::Translation(rs.size * 0.125f));
									rs.pushMod(mod);
									color = Vec4(0.0f, 0.0f, 0.0f, 0.5f);
									sprite->render(rs, rs.size * 0.75f);
									sprite->render(rs, Vec2(rs.size.x * 0.75f, 64.0f));
									rs.popMod();
									rs.popTransform();
								}

								color = Vec4(1.0f);

								rs.pushTransform();
								rs.addTransform(Matrix4::Translation(Vec2(rs.size.x * 0.125f, rs.size.y * 0.125f)));
								rs.addTransform(Matrix4::Translation(Vec3(16.0f, 80.0f, 0.0f)));
								for (auto item : items.items)
								{
									if (item)
									{
										auto type = ItemType::get(item->type);
										if (type)
										{
											if (type->dec)
											{
												glEnable(GL_DEPTH_TEST);
												glDepthMask(GL_TRUE);
												rs.pushTransform();
												rs.addTransform(Matrix4::Translation(Vec2(32.0f, 0.0f)));
												rs.addTransform(Quaternion(1.0f, Vec3(-1.0f, 1.0f, 0.0f).Normalize()));
												rs.addTransform(Matrix4::Scale(Vec3(128.0f)));
												rs.pushMod(mod3d);
												type->dec->render(rs);
												rs.popMod();
												rs.popTransform();
												glDepthMask(GL_FALSE);
												glDisable(GL_DEPTH_TEST);
											}
											auto icon = Resource::get<Texture>(type->icon);
											if (icon)
											{
												rs.pushMod(mod);
												icon->render(rs, Vec2(64.0f));
												rs.popMod();
											}
											rs.pushTransform();
											rs.addTransform(Matrix4::Translation(Vec2(64.0f, 24.0f)));
											Writing::setSize(25);
											Writing::setColor(1.0f, 1.0f, 1.0f);
											Writing::render(type->name, rs);
											rs.popTransform();
											rs.popTransform();
											rs.pushTransform();
											rs.addTransform(Matrix4::Translation(Vec2(64.0f, 40.0f)));
											Writing::setSize(15);
											Writing::setColor(1.0f, 1.0f, 1.0f, 0.8f);
											Writing::render(type->desc, rs);
											rs.popTransform();
											rs.popTransform();
											rs.addTransform(Matrix4::Translation(Vec3(0.0f, 64.0f, 0.0f)));
										}
									}
								}
								rs.popTransform();
							}
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
								GlobalPosition text_position = p->p + Vec3(0.0f, 0.0f, 0.75f);
								Vec3 dif = text_position - entity->world->cam_pos;
								if (dif.LenPwr() < 500.0f)
								{
									Vec3 screen_pos = dif * rs.view;
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
	commands.writeLog(os, client);
	interact.writeLog(os, client);
	items.writeLog(os);
	notifications.writeLog(os);
}

void InventoryComponent::readLog(instream& is)
{
	commands.readLog(is);
	interact.readLog(is);
	items.readLog(is);
	notifications.readLog(is, ClientData());
}

#include <lmcons.h>
#include <codecvt>

void InventoryComponent::writeLog(outstream& os)
{
	commands.writeLog(os);
	interact.writeLog(os);
	notifications.writeLog(os, ClientData());
}

void InventoryComponent::readLog(instream& is, ClientData& client)
{
	if (client.client_id == client_id)
	{
		commands.readLog(is, client);

		uint32_t next_sync, next_size;
		is >> next_sync >> next_size;
		std::vector<EntityID> next(next_size);
		for (size_t i = 0; i < next_size; ++i)
		{
			is >> next[i];
			if (next[i].id < client.unit_uid.size())
			{
				if (next[i].uid == client.unit_uid[next[i].id])
				{
					next[i].id = client.getRealID(next[i].id);
					if (next[i].id < entity->world->uid.size())
					{
						next[i].uid = entity->world->uid[next[i].id];
					}
					else
					{
						next[i] = EntityID();
					}
				}
				else
				{
					next[i] = EntityID();
				}
			}
			else
			{
				next[i] = EntityID();
			}
		}
		interact.update(next_sync, next);

		is >> next_sync;
		notifications.update(next_sync);
	}
}

void InventoryComponent::interpolate(Component * pComponent, float fWeight)
{
	auto other = reinterpret_cast<InventoryComponent*>(pComponent);

	interact.update(other->interact.sync);

	items = other->items;

	notifications.update(other->notifications.sync, other->notifications.queue);
	other->notifications.queue.clear();
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