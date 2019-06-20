#pragma once

#include "Component.h"

#include <functional>

class MobComponent;

class InteractComponent :
	public Component
{
public:
	InteractComponent(void);
	InteractComponent(instream& is);
	~InteractComponent(void);

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

	static AutoSerialFactory<InteractComponent, Component> _factory;

	std::function<void(MobComponent*)> func;

	std::string name, action_name;
};