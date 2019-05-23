#include "TextWindow.h"

#include "Writing.h"
#include "RenderSetup.h"

TextWindow::TextWindow(int x, int y, int w, int h) : RectangleWindow(x, y, w, h)
{
	color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);

	font_size = 16;
}

TextWindow::~TextWindow(void)
{
}

void TextWindow::render(RenderSetup& rs)
{
	rs.pushTransform();
	rs.addTransform(Matrix4::Translation(Vec2(x, y)));

	Writing::setSize(font_size);
	Writing::setColor(color);
	Writing::render(text, rs);
	rs.popTransform();

	rs.popTransform();
}