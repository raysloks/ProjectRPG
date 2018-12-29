#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include <memory>

#include "Component.h"

#include "GlobalPosition.h"

class Pose;

class LightComponent :
	public Component
{
public:
	LightComponent(void);
	LightComponent(instream& is, bool full);
	~LightComponent(void);

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

	GlobalPosition p;

	Pose * pose;

	int bone_id;

	static AutoSerialFactory<LightComponent, Component> _factory;

	static std::vector<LightComponent*> all;
};

#endif