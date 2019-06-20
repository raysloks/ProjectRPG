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
	Vec2 offset;
	Vec4 color;
	int font_size;

	void render(RenderSetup& rs);
};

#endif