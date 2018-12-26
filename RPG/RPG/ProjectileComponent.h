#ifndef PROJECTILE_COMPONENT_H
#define PROJECTILE_COMPONENT_H

#include "Component.h"

#include <functional>

#include "Vec3.h"

class MobComponent;

class ProjectileComponent :
	public Component
{
public:
	ProjectileComponent(void);
	ProjectileComponent(instream& is, bool full);
	~ProjectileComponent(void);

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

	ASF_H(ProjectileComponent, Component)

	Vec3 v;
	float drag;
	float t;
	MobComponent * shooter;

	std::function<void(MobComponent*)> on_collision;
};

#endif