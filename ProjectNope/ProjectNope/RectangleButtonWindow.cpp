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
}

#include "Texture.h"

void RectangleButtonWindow::render(RenderSetup& rs)
{
	rs.pushTransform();
	rs.addTransform(Matrix4::Translation(p));
	rs.addTransform(Matrix4::Scale(Vec3(w, h, 1.0f)));

	Vec4 color(1.0f, 1.0f, 1.0f, 0.4f);
	if (hover)
		color = Vec4(0.8f, 0.8f, 0.8f, 0.4f);
	if (pressed)
		color = Vec4(0.6f, 0.6f, 0.6f, 0.4f);
	ShaderMod mod(nullptr, [color](const std::shared_ptr<ShaderProgram>& prog) {
		prog->Uniform("color", color);
	});

	rs.pushMod(mod);

	auto image = Resource::get<Texture>("data/assets/white.tga");
	if (image)
		image->render(rs);

	rs.popMod();

	rs.popTransform();

	rs.pushTransform();
	rs.addTransform(Matrix4::Translation(Vec3(x, y + h, 0.0f) + offset));
	Writing::setSize(24);
	Writing::setColor(0.0f, 0.0f, 0.0f);
	Writing::render(text, rs);
	rs.popTransform();
	rs.popTransform();
	rs.pushTransform();
	rs.addTransform(Matrix4::Translation(Vec3(x, y, 0.0f)));
	Window::render(rs);
	rs.popTransform();
}

bool RectangleButtonWindow::handleEvent(IEvent * pEvent)
{
	if (pEvent->GetType()==MouseMoveEvent::event_type) {
		MouseMoveEvent * ev = (MouseMoveEvent*)pEvent;
		if (!ev->relative)
			if (ev->x>=x && ev->y>=y && ev->x<x+w && ev->y<y+h) {
				hover = true;
			} else {
				if (hover) {
					pressed = false;
					hover = false;
				}
			}
	}
	if (pEvent->GetType()==KeyDownEvent::event_type) {
		KeyDownEvent * ev = (KeyDownEvent*)pEvent;
		if (ev->key==Platform::KeyEvent::LMB || ev->key==Platform::KeyEvent::MMB || ev->key==Platform::KeyEvent::RMB)
		{
			if (ev->x>=x && ev->y>=y && ev->x<x+w && ev->y<y+h) {
				if (onClick)
					onClick();
				pressed = true;
				return true;
			}
		}
	}
	if (pEvent->GetType()==KeyUpEvent::event_type) {
		KeyUpEvent * ev = (KeyUpEvent*)pEvent;
		if (ev->key==Platform::KeyEvent::LMB || ev->key==Platform::KeyEvent::MMB || ev->key==Platform::KeyEvent::RMB)
		{
			pressed = false;
			/*if (ev->x>=x && ev->y>=y && ev->x<=x+w && ev->y<=y+h)
				if (!fpCallbackKeyUp.empty())
					fpCallbackKeyUp(this);*/
		}
	}
	return false;
}