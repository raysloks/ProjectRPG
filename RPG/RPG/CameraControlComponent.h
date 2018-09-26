#ifndef CAMERA_CONTROL_COMPONENT_H
#define CAMERA_CONTROL_COMPONENT_H

#include "Component.h"

#include "Vec2.h"
#include "Vec3.h"
#include "Quaternion.h"
#include "GlobalPosition.h"

class PositionComponent;

class CameraControlComponent :
	public Component
{
public:
	CameraControlComponent(void);
	CameraControlComponent(instream& is, bool full);
	~CameraControlComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void pre_frame(float dTime);
	void post_frame(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	bool visible(ClientData& client) const;

	static const AutoSerialFactory<CameraControlComponent> _factory;

	GlobalPosition p;
};

#endif