#include "GameStateComponent.h"

#include "World.h"

#include "Client.h"
#include "ClientData.h"

#include "RenderSetup.h"

#include "Writing.h"

const AutoSerialFactory<GameStateComponent> GameStateComponent::_factory("GameStateComponent");

GameStateComponent::GameStateComponent(void) : Serializable(_factory.id)
{
	game_over = false;
}

GameStateComponent::GameStateComponent(instream& is, bool full) : Serializable(_factory.id)
{
}

GameStateComponent::~GameStateComponent(void)
{
	set_display(false);
}

void GameStateComponent::connect(NewEntity * pEntity, bool authority)
{
}

void GameStateComponent::disconnect(void)
{
}

void GameStateComponent::pre_frame(float dTime)
{
	set_display(true);
}

void GameStateComponent::post_frame(float dTime)
{
}

void GameStateComponent::tick(float dTime)
{
}

void GameStateComponent::writeLog(outstream& os, ClientData& client)
{
}

void GameStateComponent::readLog(instream& is)
{
}

void GameStateComponent::writeLog(outstream& os)
{
}

void GameStateComponent::readLog(instream& is, ClientData& client)
{
}

void GameStateComponent::interpolate(Component * pComponent, float fWeight)
{
}

void GameStateComponent::write_to(outstream& os, ClientData& client) const
{
}

void GameStateComponent::write_to(outstream& os) const
{
}

void GameStateComponent::set_display(bool enable)
{
	Client * client = entity->world->client;
	if (client != nullptr)
	{
		if (enable)
		{
			if (!func)
			{
				func.reset(new std::function<void(RenderSetup&)>([this](RenderSetup& rs) {

					rs.pushTransform();
					rs.addTransform(Matrix4::Translation(Vec3(400.0f, 200.0f, 0.0f)));
					Writing::setSize(30);
					Writing::setColor(0.9f, 0.9f, 0.9f);
					Writing::render("Your score : 124\nEnemy Score : 104", rs);
					rs.popTransform();
					rs.popTransform();

				}));
				client->render2D.push_back(func);
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
