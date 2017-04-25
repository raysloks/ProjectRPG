#include "RectangleWindow.h"

RectangleWindow::RectangleWindow(int px, int py, int width, int height)
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

#include "PlatformEvents.h"
#include "GUIObject.h"
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
			focus = ev->x>=x && ev->y>=y && ev->x<=x+w && ev->y<=y+h;
			if (onClick)
				onClick();
		}
	}
	return focus;
}

void RectangleWindow::render(void)
{
	glTranslatef(x, y, 0.0f);
	Window::render();
	glTranslatef(-x, -y, 0.0f);
}