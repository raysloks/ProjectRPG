#ifndef TRIGGER_COMPONENT_H
#define TRIGGER_COMPONENT_H

#include "Component.h"

#include <functional>

#include "GlobalPosition.h"

class MobComponent;

class TriggerComponent :
	public Component
{
public:
	TriggerComponent(void);
	TriggerComponent(instream& is, bool full);
	~TriggerComponent(void);

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

	static AutoSerialFactory<TriggerComponent, Component> _factory;

	bool inside_area(const GlobalPosition& p) const;

	GlobalPosition aabb_min, aabb_max;

	float delay, countdown;

	std::function<void(MobComponent*)> func;
	std::function<void(const std::vector<MobComponent*>&)> func_multi;
};

#endif