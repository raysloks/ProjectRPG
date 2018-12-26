#include "AnimationState.h"

SerializableRegistry<AnimationState> AnimationState::_registry;

AnimationState::AnimationState(uint32_t id) : _serial_id(id)
{
}

AnimationState::~AnimationState()
{
}

SerializableRegistrationQueue<AnimationState> * AnimationState::_getRegistrationQueue()
{
	static SerializableRegistrationQueue<AnimationState> * rq = new SerializableRegistrationQueue<AnimationState>();
	return rq;
}