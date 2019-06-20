#ifndef HIT_COMPONENT_H
#define HIT_COMPONENT_H

#include "Component.h"

#include "GlobalPosition.h"
#include "EntityID.h"

#include <functional>
#include <set>

class MobComponent;

class HitComponent :
	public Component
{
public:
	HitComponent(void);
	HitComponent(instream& is);
	~HitComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void tick(float dTime);

	void writeLog(outstream& os, const std::shared_ptr<ClientData>& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, const std::shared_ptr<ClientData>& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, const std::shared_ptr<ClientData>& client) const;
	void write_to(outstream& os) const;

	static AutoSerialFactory<HitComponent, Component> _factory;

	EntityID owner;
	std::string bone, activation;
	std::function<void(MobComponent*, const Vec3&)> func;
	bool active;
	float radius, prev_r;
	Vec3 offset;
	GlobalPosition prev_p;
	std::set<MobComponent*> hit;
};

#endif