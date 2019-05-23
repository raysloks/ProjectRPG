#include "RectangleWindow.h"
#include "RenderSetup.h"

#include "Texture.h"

RectangleWindow::RectangleWindow(float px, float py, float width, float height)
{
	x = px;
	y = py;
	w = width;
	h = height;
	focus = false;
}

RectangleWindow::~RectangleWindow(void)
{
}

void RectangleWindow::render(RenderSetup& rs)
{
	rs.pushTransform();
	rs.addTransform(Matrix4::Translation(p));
	rs.pushTransform();
	rs.addTransform(Matrix4::Scale(Vec3(w, h, 1.0f)));

	Vec4 color(1.0f, 1.0f, 1.0f, 0.4f);
	ShaderMod mod(nullptr, [color](const std::shared_ptr<ShaderProgram>& prog) {
		prog->Uniform("color", color);
	});

	rs.pushMod(mod);

	auto image = Resource::get<Texture>("data/assets/white.tga");
	if (image)
		image->render(rs);

	rs.popMod();

	rs.popTransform();
	Window::render(rs);
	rs.popTransform();
}

#include "PlatformEvents.h"
#include "KeyEvent.h"
#include <iostream>

bool RectangleWindow::handleEvent(IEvent * pEvent)
{
	IEvent * pass_on = 0;
	if (pEvent->GetType()==KeyDownEvent::event_type) {
		KeyDownEvent * ev = (KeyDownEvent*)pEvent;
		pass_on = new KeyDownEvent(*ev);
		((KeyDownEvent*)pass_on)->x -= x;
		((KeyDownEvent*)pass_on)->y -= y;
	}
	if (pEvent->GetType()==KeyUpEvent::event_type) {
		KeyUpEvent * ev = (KeyUpEvent*)pEvent;
		pass_on = new KeyUpEvent(*ev);
		((KeyUpEvent*)pass_on)->x -= x;
		((KeyUpEvent*)pass_on)->y -= y;
	}
	if (pEvent->GetType()==MouseMoveEvent::event_type) {
		MouseMoveEvent * ev = (MouseMoveEvent*)pEvent;
		pass_on = new MouseMoveEvent(*ev);
		if (!ev->relative)
		{
			((MouseMoveEvent*)pass_on)->x -= x;
			((MouseMoveEvent*)pass_on)->y -= y;
		}
	}
	if (pass_on!=0)
		if (Window::handleEvent(pass_on))
			return true;
	if (pEvent->GetType()==KeyDownEvent::event_type) {
		KeyDownEvent * ev = (KeyDownEvent*)pEvent;
		if (ev->key==Platform::KeyEvent::LMB || ev->key==Platform::KeyEvent::MMB || ev->key==Platform::KeyEvent::RMB) {
			focus = ev->x>=x && ev->y>=y && ev->x<x+w && ev->y<y+h;
			if (onClick)
				onClick();
		}
	}
	//return focus;
	return false;
}