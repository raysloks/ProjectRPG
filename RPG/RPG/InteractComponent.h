#pragma once

#include "Component.h"

#include <functional>

class MobComponent;

class InteractComponent :
	public Component
{
public:
	InteractComponent(void);
	InteractComponent(instream& is, bool full);
	~InteractComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void post_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	ASF_H(InteractComponent, Component)

	std::function<void(MobComponent*)> func;

	std::string name, action_name;
};