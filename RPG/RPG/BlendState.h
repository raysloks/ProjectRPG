#pragma once

#include "SimpleState.h"

class BlendState :
	public SimpleState
{
public:
	BlendState(const std::string& name, const std::string& second_name, float weight = 0.5f, float speed = 1.0f);
	BlendState(instream& is, bool full);
	~BlendState();

	void tick(float dTime);

	void write_to(outstream& os) const;

	uint32_t getSerial() const;

	static AutoSerialFactory<BlendState, AnimationState> _factory;

	std::string second_name;
	float weight;
};