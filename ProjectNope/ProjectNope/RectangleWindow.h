#include "Window.h"

#ifndef RECTANGLE_WINDOW_H
#define RECTANGLE_WINDOW_H

class RectangleWindow :
	public Window
{
public:
	RectangleWindow(int x, int y, int width, int height);
	~RectangleWindow(void);

	bool handleEvent(IEvent * pEvent);

	void render(RenderSetup& rs);

	int x, y, w, h;
protected:
	bool focus;
};

#endif