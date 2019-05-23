#include "TextBoxWindow.h"
#include "GUIObject.h"
#include "Writing.h"

TextBoxWindow::TextBoxWindow(int x, int y, int w, int h) : TextWindow(x, y, w, h)
{
}

TextBoxWindow::~TextBoxWindow(void)
{
}

void TextBoxWindow::render(void)
{
	glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
	glBegin(GL_QUADS);

	glVertex2f(x, y);
	glVertex2f(x, y+h);
	glVertex2f(x+w, y+h);
	glVertex2f(x+w, y);

	glEnd();
	
	glTranslatef(x, y, 0.0f);
	//Writing::setSize(size);
	Writing::setColor(color.x, color.y, color.z, color.w);
	/*if (text.size())
		Writing::render(text);
	else
		Writing::render(placeholder);*/

	glTranslatef(-x, -y, 0.0f);

	//RectangleWindow::render();

	glDisable(GL_TEXTURE_2D);

	t += 0.025f;

	if (t>2.0f)
		t-=2.0f;

	if (t<1.0f && focus)
	{
		glPushMatrix();

		Writing::getAdvance(text.substr(0, caret));

		glBegin(GL_QUADS);

		glVertex2f(x-1, y-4);
		glVertex2f(x-1, y+font_size+4);
		glVertex2f(x+1, y+font_size+4);
		glVertex2f(x+1, y-4);

		glEnd();

		glPopMatrix();
	}
}

#include "PlatformEvents.h"

bool TextBoxWindow::handleEvent(IEvent * pEvent)
{
	if (pEvent->GetType()==KeyDownEvent::event_type) {
		KeyDownEvent * ev = (KeyDownEvent*)pEvent;
		if (ev->key==Platform::KeyEvent::LMB || ev->key==Platform::KeyEvent::MMB || ev->key==Platform::KeyEvent::RMB)
			focus = ev->x>=x && ev->y>=y && ev->x<=x+w && ev->y<=y+h;
	}
	if (focus)
	{
		if (pEvent->GetType()==KeyDownEvent::event_type) {
			KeyDownEvent * ev = (KeyDownEvent*)pEvent;
			bool action = false;
			if (ev->key==Platform::KeyEvent::LMB)
			{
				caret = (ev->x-x)/size.x;
				action = true;
			}
			if (ev->key==Platform::KeyEvent::LEFT) {
				if (caret!=0) {
					caret--;
				}
				caret = std::distance(text.begin(), Writing::getRange(text.begin(), text.begin() + caret, text.end()).first);
				action = true;
			}
			if (ev->key==Platform::KeyEvent::RIGHT) {
				caret = std::distance(text.begin(), Writing::getRange(text.begin(), text.begin() + caret, text.end()).second);
				action = true;
			}
			if (ev->key==Platform::KeyEvent::DEL) {
				if (caret<text.size())
					caret = std::distance(text.begin(), Writing::erase(text, text.begin()+caret));
				action = true;
			}
			if (ev->chr.size())
			{
				if (ev->chr.front()==(unsigned char)8) {
					if (text.size()) {
						if (caret!=0)
						{
							caret = std::distance(text.begin(), Writing::erase(text, text.begin()+caret-1));
						}
					}
					action = true;
				}
				if (ev->chr.front()==(unsigned char)9) {
					//tab
				}
				if (ev->chr.front()==(unsigned char)13) {
					//enter
				}
				if (ev->chr.front()==(unsigned char)27) {
					action = true;
				}
				if (!action) {
					if (caret<text.size())
						text.insert(caret, ev->chr);
					else
						text.append(ev->chr);
					caret += ev->chr.size();
					action = true;
				}
			}
			if (action)
			{
				t = 0.5f;
				return true;
			}
		}
	}
	return RectangleWindow::handleEvent(pEvent);
}