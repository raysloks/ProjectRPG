#ifndef TEXT_WINDOW_H
#define TEXT_WINDOW_H

#include "Vec2.h"
#include "Vec4.h"
#include "RectangleWindow.h"

class TextWindow :
	public RectangleWindow
{
public:
	TextWindow(int x, int y, int w, int h);
	~TextWindow(void);

	std::string text;
	Vec4 color;
	Vec2 size;

	void render(void);
};

#endif