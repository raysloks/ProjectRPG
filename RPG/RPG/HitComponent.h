#ifndef HIT_COMPONENT_H
#define HIT_COMPONENT_H

#include "Component.h"

#include "GlobalPosition.h"

#include <functional>
#include <set>

class MobComponent;

class HitComponent :
	public Component
{
public:
	HitComponent(void);
	HitComponent(instream& is, bool full);
	~HitComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	static const AutoSerialFactory<HitComponent> _factory;

	std::function<void(MobComponent*, const Vec3&)> func;
	bool active;
	float r;
	GlobalPosition prev_p;
	std::set<MobComponent*> hit;
};

#endif