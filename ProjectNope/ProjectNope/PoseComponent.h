#ifndef POSE_COMPONENT_H
#define POSE_COMPONENT_H

#include "Component.h"

#include "SkeletalAnimation.h"

class PoseComponent :
	public Component
{
public:
	PoseComponent(void);
	PoseComponent(instream& is, bool full);
	~PoseComponent(void);

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

	static const AutoSerialFactory<PoseComponent> _factory;

	float frame;
	std::shared_ptr<Pose> pose;
	std::string anim;
};

#endif