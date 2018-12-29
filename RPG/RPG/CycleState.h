#pragma once

#include "SimpleState.h"

class CycleState :
	public SimpleState
{
public:
	CycleState(const std::string& name, float speed = 1.0f);
	CycleState(instream& is, bool full);
	~CycleState();

	void enter(AnimationState * prev);
	void tick(float dTime);
	void leave(AnimationState * next);

	void interpolate(AnimationState * other, float fWeight);

	void write_to(outstream& os) const;

	static AutoSerialFactory<CycleState, AnimationState> _factory;
};