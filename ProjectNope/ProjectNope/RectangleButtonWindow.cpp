#include "RectangleButtonWindow.h"
#include "PlatformEvents.h"
#include "GUIObject.h"
#include "Writing.h"

RectangleButtonWindow::RectangleButtonWindow(int px, int py, int width, int height) : RectangleWindow(px, py, width, height)
{
	hover = false;
	pressed = false;

	offset = Vec2(8.0f, 22.0f);
}

RectangleButtonWindow::~RectangleButtonWindow(void)
{
}

bool RectangleButtonWindow::handleEvent(IEvent * pEvent)
{
	if (pEvent->GetType()==MouseMoveEvent::event_type) {
		MouseMoveEvent * ev = (MouseMoveEvent*)pEvent;
		if (!ev->relative)
			if (ev->x>=x && ev->y>=y && ev->x<=x+w && ev->y<=y+h) {
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
			if (ev->x>=x && ev->y>=y && ev->x<=x+w && ev->y<=y+h) {
				if (!onClick.empty())
					onClick();
				pressed = true;
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

void RectangleButtonWindow::render(void)
{
	glDisable(GL_TEXTURE_2D);

	glBegin(GL_TRIANGLES);

	/*if (pressed)
		glColor3f(1.0f, 1.0f, 1.0f);
	else {
		glColor3f(0.0f, 0.0f, 0.0f);
		if (hover)
			glColor3f(0.5f, 0.5f, 0.0f);
	}
	glVertex2f(x+w, y);
	glVertex2f(x, y+h);
	glVertex2f(x+w, y+h);

	if (pressed)
		glColor3f(0.0f, 0.0f, 0.0f);
	else {
		glColor3f(1.0f, 1.0f, 1.0f);
		if (hover)
			glColor3f(1.0f, 1.0f, 1.0f);
	}
	glVertex2f(x, y);
	glVertex2f(x+w, y);
	glVertex2f(x, y+h);*/

	if (hover)
		glColor4f(1.0f, 1.0f, 1.0f, 0.75f);
	else
		glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
	glVertex2f(x+w, y);
	glVertex2f(x+w, y+h);
	glVertex2f(x, y+h);
	glVertex2f(x, y);
	glVertex2f(x+w, y);
	glVertex2f(x, y+h);

	glEnd();

	Writing::setFont("data/assets/fonts/Lora-Regular.ttf");
	Writing::setColor(0.0f, 0.0f, 0.0f, 1.0f);
	//Writing::setSize(Vec2(8.0f, 16.0f));

	glTranslatef(x+offset.x, y+offset.y, 0.0f);
	//Writing::render(text);
	glTranslatef(-x-offset.x, -y-offset.y, 0.0f);

	RectangleWindow::render();
}