#ifndef RIGID_BODY_COMPONENT_H
#define RIGID_BODY_COMPONENT_H

#include "Component.h"
#include "Vec3.h"
#include "Quaternion.h"
#include "Matrix3.h"

class GlobalPosition;

class RigidBodyComponent :
	public Component
{
public:
	RigidBodyComponent(void);
	RigidBodyComponent(instream& is, bool full);
	~RigidBodyComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void tick(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	ASF_H(RigidBodyComponent, Component)

	GlobalPosition * p;
	Vec3 v, move, w, rot;
	Quaternion q;
	Matrix3 inertia;
	float m;
};

#endif