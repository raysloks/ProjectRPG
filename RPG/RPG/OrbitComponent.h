#ifndef ORBIT_COMPONENT_H
#define ORBIT_COMPONENT_H

#include "Component.h"

#include "GlobalPosition.h"

class OrbitComponent :
	public Component
{
public:
	OrbitComponent(void);
	OrbitComponent(instream& is, bool full);
	~OrbitComponent(void);

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

	static AutoSerialFactory<OrbitComponent, Component> _factory;

	GlobalPosition center;
	Vec3 angle, offset;
	float t, period, speed;
};

#endif