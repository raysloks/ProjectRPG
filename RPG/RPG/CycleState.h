#pragma once

#include "SimpleState.h"

class CycleState :
	public SimpleState
{
public:
	CycleState(const std::string& name, float speed = 1.0f);
	CycleState(instream& is, bool full);
	~CycleState();

	void tick(float dTime);

	void interpolate(AnimationState * other, float fWeight);

	void write_to(outstream& os) const;

	uint32_t getSerial() const;

	static AutoSerialFactory<CycleState, AnimationState> _factory;
};