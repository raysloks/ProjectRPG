#ifndef RECTANGLE_WINDOW_H
#define RECTANGLE_WINDOW_H

#include "Window.h"

#include "Vec2.h"

class RectangleWindow :
	public Window
{
public:
	RectangleWindow(float x, float y, float width, float height);
	~RectangleWindow(void);

	bool handleEvent(IEvent * pEvent);

	void render(RenderSetup& rs);

	union
	{
		Vec2 p;
		struct
		{
			float x, y;
		};
	};
	union
	{
		Vec2 size;
		struct
		{
			float w, h;
		};
	};
protected:
	bool focus;
};

#endif