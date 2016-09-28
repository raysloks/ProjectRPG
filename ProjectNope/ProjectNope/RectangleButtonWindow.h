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

	virtual bool handleEvent(IEvent * pEvent);

	virtual void render(void);

	std::string text;

protected:
	bool hover, pressed;

	Vec2 offset;
};

#endif