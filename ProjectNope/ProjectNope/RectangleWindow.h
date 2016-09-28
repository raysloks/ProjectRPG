#include "Window.h"

#ifndef RECTANGLE_WINDOW_H
#define RECTANGLE_WINDOW_H

class RectangleWindow :
	public Window
{
public:
	RectangleWindow(int x, int y, int width, int height);
	virtual ~RectangleWindow(void);

	virtual bool handleEvent(IEvent * pEvent);

	virtual void render(void);

	int x, y, w, h;
protected:
	bool focus;
};

#endif