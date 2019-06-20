#include "RectangleButtonWindow.h"
#include "PlatformEvents.h"
#include "GUIObject.h"
#include "Writing.h"

RectangleButtonWindow::RectangleButtonWindow(int px, int py, int width, int height) : RectangleWindow(px, py, width, height)
{
	hover = false;
	pressed = false;

	offset = Vec2(8.0f, -8.0f);
}

RectangleButtonWindow::~RectangleButtonWindow(void)
{
	RectangleWindow::~RectangleWindow();
}

#include "Texture.h"

void RectangleButtonWindow::render(RenderSetup& rs)
{
	rs.pushTransform();
	rs.addTransform(Matrix4::Translation(min));

	Vec4 color(1.0f, 1.0f, 1.0f, 0.4f);
	if (hover)
		color = Vec4(0.7f, 0.7f, 0.7f, 0.4f);
	if (pressed)
		color = Vec4(0.4f, 0.4f, 0.4f, 0.4f);
	ShaderMod mod(nullptr, [color](const std::shared_ptr<ShaderProgram>& prog) {
		prog->Uniform("color", color);
	});

	rs.pushMod(mod);

	auto image = Resource::get<Texture>("data/assets/white.tga");
	if (image)
		image->render(rs, size);

	rs.popMod();

	rs.popTransform();

	rs.pushTransform();
	rs.addTransform(Matrix4::Translation(Vec2(min.x, max.y) + offset));
	Writing::setSize(24);
	Writing::setColor(0.0f, 0.0f, 0.0f);
	Writing::setOffset(Vec2());
	Writing::render(text, rs);
	rs.popTransform();
	rs.popTransform();

	Window::render(rs);
}

bool RectangleButtonWindow::handleEvent(IEvent * pEvent)
{
	if (pEvent->GetType()==MouseMoveEvent::event_type)
	{
		MouseMoveEvent * ev = (MouseMoveEvent*)pEvent;
		if (!ev->relative)
		{
			if (ev->x >= min.x && ev->y >= min.y && ev->x < max.x && ev->y < max.y)
			{
				hover = true;
			}
			else
			{
				pressed = false;
				hover = false;
			}
		}
	}
	if (pEvent->GetType()==KeyDownEvent::event_type)
	{
		KeyDownEvent * ev = (KeyDownEvent*)pEvent;
		if (ev->key==Platform::KeyEvent::LMB || ev->key==Platform::KeyEvent::MMB || ev->key==Platform::KeyEvent::RMB)
		{
			if (ev->x >= min.x && ev->y >= min.y && ev->x < max.x && ev->y < max.y)
			{
				pressed = true;
			}
		}
	}
	if (pEvent->GetType()==KeyUpEvent::event_type)
	{
		KeyUpEvent * ev = (KeyUpEvent*)pEvent;
		if (ev->key==Platform::KeyEvent::LMB || ev->key==Platform::KeyEvent::MMB || ev->key==Platform::KeyEvent::RMB)
		{
			pressed = false;
		}
	}
	return Window::handleEvent(pEvent);
}