#ifndef RECTANGLE_WINDOW_H
#define RECTANGLE_WINDOW_H

#include "Window.h"

#include "Vec4.h"

class RectangleWindow :
	public Window
{
public:
	RectangleWindow(float x, float y, float width, float height);
	~RectangleWindow(void);

	void render(RenderSetup& rs);

	Vec4 color;
	std::string background;
};

#endif