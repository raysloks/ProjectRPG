#pragma once

#include "CycleState.h"

class RunCycleState :
	public CycleState
{
public:
	RunCycleState(const std::string& name, float speed, const std::string& idle_name, float idle_speed);
	RunCycleState(instream& is);
	~RunCycleState();

	void enter(AnimationState * prev);
	void tick(float dTime);
	void leave(AnimationState * next);

	void interpolate(AnimationState * other, float fWeight);

	void write_to(outstream& os) const;

	uint32_t getSerial() const;

	static AutoSerialFactory<RunCycleState, AnimationState> _factory;

	CycleState idle;
};