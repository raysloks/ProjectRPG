#pragma once

#include "AutoSerialFactory.h"
#include "SerializableRegistry.h"

class AnimationControlComponent;
class PoseComponent;
class MobComponent;

class AnimationState
{
public:
	AnimationState();
	virtual ~AnimationState() = 0;

	virtual void enter(AnimationState * prev) = 0;
	virtual void tick(float dTime) = 0;
	virtual void leave(AnimationState * next) = 0;

	virtual void interpolate(AnimationState * other, float fWeight) = 0;

	virtual void write_to(outstream& os) const = 0;

	virtual uint32_t getSerial() const = 0;

	static SerializableRegistry<AnimationState> _registry;

	AnimationControlComponent * acc;
	PoseComponent * pose;
	MobComponent * mob;
	bool authority;
};