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
							Writing::setColor(0.0f, 1.0f, 0.0f);
							Writing::render(std::to_string((int)std::floorf(mob->health.current)) + " / " + std::to_string((int)std::floorf(mob->health.max)), rs);
							rs.popTransform();
							rs.popTransform();

							// mana
							rs.pushTransform();
							rs.addTransform(Matrix4::Translation(Vec3(100.0f, 140.0f, 0.0f)));
							Writing::setSize(25);
							Writing::setColor(0.0f, 0.0f, 1.0f);
							Writing::render(std::to_string((int)std::floorf(mob->mana.current)) + " / " + std::to_string((int)std::floorf(mob->mana.max)), rs);
							rs.popTransform();
							rs.popTransform();

							if (false)
							{
								// reticle
								rs.pushTransform();
								rs.addTransform(Matrix4::Translation(Vec3(rs.size / 2.0f)));
								rs.addTransform(Matrix4::Translation(Vec3(-32.0f, -32.0f, 0.0f)));
								auto crosshair = Resource::get<Texture>("data/assets/crosshair.tga");
								if (crosshair)
									crosshair->render(rs);
								rs.popTransform();
							}

							rs.pushTransform();
							rs.addTransform(Matrix4::Translation(Vec3(100.0f, 200.0f, 0.0f)));
							for (auto item : items.items)
							{
								// test
								rs.addTransform(Matrix4::Translation(Vec3(0.0f, 50.0f, 0.0f)));

								rs.pushTransform();
								rs.addTransform(Matrix4::Translation(Vec3(20.0f, 10.0f, 0.0f)));
								Writing::setColor(1.0f, 1.0f, 1.0f);
								Writing::render(item->name, rs);
								rs.popTransform();
								rs.popTransform();

								rs.pushTransform();
								rs.addTransform(Matrix4::Scale(Vec3(50.0f, 50.0f, 50.0f)));
								rs.addTransform(Quaternion(Vec3(M_1_PI, 0.0f, 0.0f)));
								rs.addTransform(Quaternion(Vec3(0.0f, rot, 0.0f)));
								auto prog = ShaderProgram::Get("data/gfill_vert.txt", "data/gfill_frag.txt");
								ShaderMod mod(prog, [](const std::shared_ptr<ShaderProgram>& prog) {
									prog->Uniform("diffuse", 0); // texture unit 0
									prog->Uniform("light", Vec3(1.0f, -1.0f, -1.0f).Normalize());
									prog->Uniform("eye", Vec3(0.0f, 0.0f, -1.0f));
								});
								rs.pushMod(mod);
								glEnable(GL_DEPTH_TEST);
								glEnable(GL_CULL_FACE);
								glDepthMask(GL_TRUE);
								item->dec->render(rs);
								glDepthMask(GL_FALSE);
								glDisable(GL_DEPTH_TEST);
								glDisable(GL_CULL_FACE);
								rs.popMod();
								rs.popTransform();
							}
							rs.popTransform();

							auto p = entity->getComponent<PositionComponent>();
							auto interacts = entity->world->GetNearestComponents<InteractComponent>(p->p + mob->facing, 2.0f);
							if (!interacts.empty())
							{
								auto other_p = interacts.begin()->second->entity->getComponent<PositionComponent>();
								rs.pushTransform();
								rs.addTransform(Matrix4::Translation(Vec3(rs.size * 0.5f)));
								rs.addTransform(Matrix4::Translation(Vec3(Vec3(other_p->p - entity->world->cam_pos) * rs.view * Vec3(1.0f, -1.0f, 0.0f) * rs.size * 0.5f)));
								rs.pushTransform();
								rs.addTransform(Matrix4::Translation(Vec3(20.0f, 0.0f, 0.0f)));
								Writing::setSize(25);
								Writing::setColor(1.0f, 1.0f, 1.0f);
								Writing::render(interacts.begin()->second->action_name, rs);
								rs.popTransform();
								rs.popTransform();
								rs.addTransform(Matrix4::Translation(Vec3(-32.0f, -32.0f, 0.0f)));
								auto crosshair = Resource::get<Texture>("data/assets/interact.tga");
								if (crosshair)
									crosshair->render(rs);
								rs.popTransform();
							}
						}));
					}
					else
					{
						func.reset(new std::function<void(RenderSetup&)>([this, mob](RenderSetup& rs) {
							/*rs.pushTransform();
							rs.transform = Matrix4();
							if (mob->p)
								rs.addTransform(Matrix4::Translation(*mob->p - rs.origin));
							rs.addTransform(rs.view);
							Writing::setSize(25);
							Writing::setColor(0.3f, 0.5f, 0.1f);
							Writing::render("Ally guy test", rs);
							rs.popTransform();
							rs.popTransform();*/
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