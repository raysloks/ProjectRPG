#include "ChatComponent.h"

#include "World.h"

#include "Client.h"
#include "ClientData.h"

#include "RenderSetup.h"

#include "Writing.h"

#include "MobComponent.h"

outstream& operator<<(outstream& os, const ChatMessage& msg)
{
	os << msg.timeout << msg.message;
	return os;
}

instream& operator>>(instream& is, ChatMessage& msg)
{
	is >> msg.timeout >> msg.message;
	return is;
}

AutoSerialFactory<ChatComponent, Component> ChatComponent::_factory("ChatComponent");

ChatComponent::ChatComponent(void) : Component(_factory.id)
{
}

ChatComponent::ChatComponent(instream& is) : Component(_factory.id)
{
}

ChatComponent::~ChatComponent(void)
{
	if (func)
		set_display(false);
}

void ChatComponent::connect(NewEntity * pEntity, bool authority)
{
}

void ChatComponent::disconnect(void)
{
}

void ChatComponent::pre_frame(float dTime)
{
	set_display(true);
}

void ChatComponent::post_frame(float dTime)
{
}

void ChatComponent::tick(float dTime)
{
	if (entity->world->authority)
	{
		if (messages.size() > 10)
			messages.erase(messages.begin(), messages.end() - 10);
		for (auto i = messages.begin(); i != messages.end();)
		{
			i->timeout -= dTime;
			if (i->timeout <= 0.0f)
			{
				i = messages.erase(i);
			}
			else
			{
				++i;
			}
		}
	}
}

void ChatComponent::writeLog(outstream& os, const std::shared_ptr<ClientData>& client)
{
	uint8_t n_messages = messages.size();
	os << n_messages;
	for (size_t i = 0; i < n_messages; i++)
	{
		os << messages[i];
	}
}

void ChatComponent::readLog(instream& is)
{
	uint8_t n_messages;
	is >> n_messages;
	messages.resize(n_messages);
	for (size_t i = 0; i < n_messages; i++)
	{
		is >> messages[i];
	}
}

void ChatComponent::writeLog(outstream& os)
{
	if (new_messages.size())
	{
		os << new_messages.front().message;
		new_messages.erase(new_messages.begin());
	}
}

void ChatComponent::readLog(instream& is, const std::shared_ptr<ClientData>& client)
{
	std::string message;
	is >> message;
	ChatMessage chat_message;
	chat_message.message = std::to_string(client->client_id) + ": " + message + "\n";
	chat_message.timeout = 10.0f;
	messages.push_back(chat_message);
}

void ChatComponent::interpolate(Component * pComponent, float fWeight)
{
	ChatComponent * other = reinterpret_cast<ChatComponent*>(pComponent);
	messages = other->messages;
}

void ChatComponent::write_to(outstream& os, const std::shared_ptr<ClientData>& client) const
{
}

void ChatComponent::write_to(outstream& os) const
{
}

void ChatComponent::set_display(bool enable)
{
	Client * client = entity->world->client;
	if (client != nullptr)
	{
		if (enable)
		{
			if (!func)
			{
				func = std::make_shared<std::function<void(RenderSetup&)>>([this](RenderSetup& rs) {
					rs.pushTransform();

					rs.addTransform(Matrix4::Translation(Vec2(20.0f, rs.size.y - 20.0f)));
					Writing::setSize(16);
					Writing::setColor(1.0f, 1.0f, 1.0f);
					for (size_t i = 0; i < messages.size(); i++)
					{
						Writing::render(messages[i].message, rs);
						rs.popTransform();
						rs.addTransform(Matrix4::Translation(Vec2(0.0f, -20.0f)));
					}

					rs.popTransform();
				});
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