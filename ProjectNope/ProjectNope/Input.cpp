#include "Input.h"

Input::Input()
{
}

Input::~Input()
{
}

void Input::update()
{
	for (int i=0;i<4;++i)
		ctrl[i].update(Platform::GUIObject::controllerState[i]);
}

void Input::clear()
{
	mouse_dif_x = 0;
	mouse_dif_y = 0;
	mouse_dif_z = 0;

	keys_pressed.clear();
	keys_released.clear();
}

bool Input::isPressed(int key) const
{
	return keys_pressed.find(key)!=keys_pressed.end();
}

bool Input::isDown(int key) const
{
	return keys_down.find(key)!=keys_down.end();
}

bool Input::isReleased(int key) const
{
	return keys_released.find(key)!=keys_released.end();
}

void Input::onDown(int key)
{
	if (!isDown(key))
	{
		keys_down.insert(key);
		keys_pressed.insert(key);
	}
}

void Input::onUp(int key)
{
	if (isDown(key))
	{
		keys_down.erase(key);
		keys_released.insert(key);
	}
}