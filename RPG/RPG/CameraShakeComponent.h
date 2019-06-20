#ifndef CAMERA_SHAKE_COMPONENT_H
#define CAMERA_SHAKE_COMPONENT_H

#include "Component.h"

#include "Vec3.h"

class CameraShakeComponent :
	public Component
{
public:
	CameraShakeComponent(void);
	CameraShakeComponent(instream& is);
	~CameraShakeComponent(void);

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

	static AutoSerialFactory<CameraShakeComponent, Component> _factory;

	Vec3 getShake(void) const;

	float t;
};

#endif