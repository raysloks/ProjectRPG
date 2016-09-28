#ifndef CONTROLLER_STATE_H
#define CONTROLLER_STATE_H

#include "Vec2.h"
#include "GUIObject.h"

class AnalogState
{
public:
	AnalogState();
	~AnalogState();

	void update(const Vec2& raw);

	Vec2 raw;

	Vec2 out, direction;
	float magnitude;
	
	float deadzone, deadzone_dir_x, deadzone_dir_y;
};

class TriggerState
{
public:
	TriggerState();
	~TriggerState();

	void update(float raw);

	float raw;

	float out;
	bool pressed, released;

	float deadzone;
};

class ButtonState
{
public:
	ButtonState();
	~ButtonState();

	void update(bool down);

	bool down, pressed, released;
};

class ControllerState
{
public:
	ControllerState();
	~ControllerState();

	void update(const XINPUT_STATE& state);

	AnalogState left_analog, right_analog;
	TriggerState left_trigger, right_trigger;

	ButtonState up, down, left, right;
	ButtonState start, back, left_thumb, right_thumb, left_shoulder, right_shoulder;
	ButtonState a, b, x, y;
};

#endif