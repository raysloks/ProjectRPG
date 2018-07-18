#pragma once

#include "AnimationState.h"

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

	void write_to(outstream& os, bool full) const;

	float t, speed;
	std::string name;
	std::map<float, std::function<void(void)>> events;

	static const AutoSerialFactory<SimpleState> _factory;
};