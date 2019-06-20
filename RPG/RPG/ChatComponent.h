#ifndef CHAT_COMPONENT_H
#define CHAT_COMPONENT_H

#include "Component.h"

#include <memory>
#include <functional>

class RenderSetup;

class ChatMessage
{
public:
	float timeout;
	std::string message;
};

outstream& operator<<(outstream& os, const ChatMessage& msg);
instream& operator>>(instream& is, ChatMessage& msg);

class ChatComponent :
	public Component
{
public:
	ChatComponent(void);
	ChatComponent(instream& is);
	~ChatComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void post_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, const std::shared_ptr<ClientData>& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;

	static AutoSerialFactory<ChatComponent, Component> _factory;

	void set_display(bool enable);

	std::shared_ptr<std::function<void(RenderSetup&)>> func;

	std::vector<ChatMessage> new_messages;
	std::vector<ChatMessage> messages;
};

#endif