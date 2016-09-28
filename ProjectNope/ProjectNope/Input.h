#ifndef INPUT_H
#define INPUT_H

#include "ControllerState.h"

#include <set>

class Input
{
public:
	Input();
	~Input();

	void update();
	void clear();

	bool isPressed(int key) const;
	bool isDown(int key) const;
	bool isReleased(int key) const;

	void onDown(int key);
	void onUp(int key);

	int mouse_x, mouse_y;
	int mouse_dif_x, mouse_dif_y, mouse_dif_z;

	ControllerState ctrl[4];

	std::set<int> keys_pressed, keys_down, keys_released;
};

#endif