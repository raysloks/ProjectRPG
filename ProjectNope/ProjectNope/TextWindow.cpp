#include "TextWindow.h"
#include "GUIObject.h"
#include "Writing.h"

TextWindow::TextWindow(int x, int y, int w, int h) : RectangleWindow(x, y, w, h)
{
	color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

	size = Vec2(8.0f, 16.0f);
}

TextWindow::~TextWindow(void)
{
}

void TextWindow::render(void)
{
	glTranslatef(x, y, 0.0f);
	//Writing::setSize(size);
	Writing::setColor(color.x, color.y, color.z, color.w);
	//Writing::render(text);

	glTranslatef(-x, -y, 0.0f);

	//RectangleWindow::render();
}