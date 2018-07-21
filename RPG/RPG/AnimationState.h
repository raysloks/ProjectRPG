#pragma once

#include "Serializable.h"
#include "AutoSerialFactory.h"

class AnimationControlComponent;
class PoseComponent;
class MobComponent;

class AnimationState :
	public virtual Serializable
{
public:
	AnimationState();
	virtual ~AnimationState();

	virtual void enter(AnimationState * prev) = 0;
	virtual void tick(float dTime) = 0;
	virtual void leave(AnimationState * next) = 0;

	virtual void interpolate(AnimationState * other, float fWeight) = 0;

	AnimationControlComponent * acc;
	PoseComponent * pose;
	MobComponent * mob;
	bool authority;
};