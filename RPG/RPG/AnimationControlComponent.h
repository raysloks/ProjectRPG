#ifndef ANIMATION_CONTROL_COMPONENT_H
#define ANIMATION_CONTROL_COMPONENT_H

#include "Component.h"
#include "Vec3.h"
#include "Matrix4.h"

class PositionComponent;
class AnimationState;

class AnimationControlComponent :
	public Component
{
public:
	AnimationControlComponent(void);
	AnimationControlComponent(instream& is, bool full);
	~AnimationControlComponent(void);

	void connect(NewEntity * pEntity, bool authority);
	void disconnect(void);

	void tick(float dTime);
	void pre_frame(float dTime);

	void writeLog(outstream& os, ClientData& client);
	void readLog(instream& is);

	void writeLog(outstream& os);
	void readLog(instream& is, ClientData& client);

	void interpolate(Component * pComponent, float fWeight);

	void write_to(outstream& os, ClientData& client) const;
	void write_to(outstream& os) const;

	ASF_H(AnimationControlComponent, Component)

	void set_state(AnimationState * new_state);

	bool has_state(const std::string& name);

	Vec3 root;
	uint32_t sync;
	AnimationState * state;
	float overtime;
	std::vector<AnimationState*> removed_states;

	float scale;
	Matrix4 transform;
};

#endif