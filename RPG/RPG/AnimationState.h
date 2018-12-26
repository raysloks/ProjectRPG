#pragma once

#include "AutoSerialFactory.h"
#include "SerializableRegistry.h"

class AnimationControlComponent;
class PoseComponent;
class MobComponent;

class AnimationState
{
public:
	AnimationState(uint32_t id);
	virtual ~AnimationState() = 0;

	virtual void enter(AnimationState * prev) = 0;
	virtual void tick(float dTime) = 0;
	virtual void leave(AnimationState * next) = 0;

	virtual void interpolate(AnimationState * other, float fWeight) = 0;

	virtual void write_to(outstream& os) const = 0;

	uint32_t _serial_id;

	static SerializableRegistry<AnimationState> _registry;
	static SerializableRegistrationQueue<AnimationState> * _getRegistrationQueue();

	AnimationControlComponent * acc;
	PoseComponent * pose;
	MobComponent * mob;
	bool authority;
};