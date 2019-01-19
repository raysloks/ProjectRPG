#pragma once

#include "AnimationState.h"

#include "SkeletalAnimation.h"

#include <string>
#include <map>
#include <functional>

class SimpleState :
	public AnimationState
{
public:
	SimpleState(const std::string& name, float speed = 1.0f);
	SimpleState(instream& is, bool full);
	~SimpleState();

	void enter(AnimationState * prev);
	void tick(float dTime);
	void leave(AnimationState * next);

	void interpolate(AnimationState * other, float fWeight);

	void write_to(outstream& os) const;

	uint32_t getSerial() const;

	static AutoSerialFactory<SimpleState, AnimationState> _factory;

	Pose prev_pose;
	float blend_t;
	float t, prev_t;
	float speed;
	std::string name;
	std::map<float, std::function<void(void)>> events;
};