#ifndef RECTANGLE_BUTTON_WINDOW_H
#define RECTANGLE_BUTTON_WINDOW_H

#include "RectangleWindow.h"
#include "Vec2.h"

class RectangleButtonWindow :
	public RectangleWindow
{
public:
	RectangleButtonWindow(int x, int y, int width, int height);
	~RectangleButtonWindow(void);

	void render(RenderSetup& rs);

	bool handleEvent(IEvent * pEvent);

	std::string text;

protected:
	bool hover, pressed;

	Vec2 offset;
};

#endif