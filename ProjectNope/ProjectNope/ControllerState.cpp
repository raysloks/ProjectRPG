#include "ControllerState.h"

AnalogState::AnalogState()
{
}

AnalogState::~AnalogState()
{
}

void AnalogState::update(const Vec2& r)
{
	out = r;
	magnitude = out.Len();
	direction = out/magnitude;
	float sign_x = signbit(direction.x) ? -1.0f : 1.0f;
	direction.x -= deadzone_dir_x * sign_x;
	if (direction.x * sign_x < 0.0f)
		direction.x = 0.0f;
	direction.x /= 1.0f - deadzone_dir_x;
	float sign_y = signbit(direction.y) ? -1.0f : 1.0f;
	direction.y -= deadzone_dir_y * sign_y;
	if (direction.y * sign_y < 0.0f)
		direction.y = 0.0f;
	direction.y /= 1.0f - deadzone_dir_y;
	direction.Normalize();
	if (magnitude>deadzone)
	{
		magnitude -= deadzone;
		magnitude /= 1.0f-deadzone;
		if (magnitude>1.0f)
			magnitude=1.0f;
	}
	else
	{
		magnitude = 0.0f;
		direction = Vec2();
	}
	out = direction*magnitude;
	
	raw = r;
}

TriggerState::TriggerState()
{
}

TriggerState::~TriggerState()
{
}

void TriggerState::update(float r)
{
	float prev = out;

	out = r;

	if (out>deadzone)
	{
		out -= deadzone;
		out /= 1.0f-deadzone;
	}
	else
	{
		out = 0.0f;
	}

	pressed = out!=0.0f && prev==0.0f;
	released = out==0.0f && prev!=0.0f;
}

ButtonState::ButtonState()
{
}

ButtonState::~ButtonState()
{
}

void ButtonState::update(bool d)
{
	pressed = d && !down;
	released = !d && down;
	down = d;
}

ControllerState::ControllerState()
{
	left_analog.deadzone = 0.5f;
	left_analog.deadzone_dir_x = 0.1f;
	left_analog.deadzone_dir_y = 0.1f;

	right_analog.deadzone = 0.2f;
	right_analog.deadzone_dir_x = 0.1f;
	right_analog.deadzone_dir_y = 0.1f;

	left_trigger.deadzone = 0.125f;
	right_trigger.deadzone = 0.125f;
}

ControllerState::~ControllerState()
{
}

void ControllerState::update(const XINPUT_STATE& state)
{
	left_analog.update(Vec2(state.Gamepad.sThumbLX, state.Gamepad.sThumbLY)/32767);
	right_analog.update(Vec2(state.Gamepad.sThumbRX, state.Gamepad.sThumbRY)/32767);
	left_trigger.update(state.Gamepad.bLeftTrigger/255);
	right_trigger.update(state.Gamepad.bRightTrigger/255);

	up.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_DPAD_UP)!=0);
	down.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_DPAD_DOWN)!=0);
	left.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_DPAD_LEFT)!=0);
	right.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_DPAD_RIGHT)!=0);

	start.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_START)!=0);
	back.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_BACK)!=0);
	left_thumb.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_LEFT_THUMB)!=0);
	right_thumb.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_RIGHT_THUMB)!=0);
	left_shoulder.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_LEFT_SHOULDER)!=0);
	right_shoulder.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_RIGHT_SHOULDER)!=0);

	a.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_A)!=0);
	b.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_B)!=0);
	x.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_X)!=0);
	y.update((state.Gamepad.wButtons&XINPUT_GAMEPAD_Y)!=0);
}