#include "TextBoxWindow.h"

#include "Writing.h"
#include "RenderSetup.h"
#include "Texture.h"

TextBoxWindow::TextBoxWindow(int x, int y, int w, int h) : TextWindow(x, y, w, h)
{
}

TextBoxWindow::~TextBoxWindow(void)
{
}

void TextBoxWindow::render(RenderSetup& rs)
{
	rs.pushTransform();
	rs.addTransform(Matrix4::Translation(min));

	Vec4 color(0.5f, 0.5f, 0.5f, 1.0f);
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
	rs.addTransform(Matrix4::Translation(Vec2(min.x, max.y)));
	Writing::setSize(size.y);
	Writing::setColor(color);
	if (text.size() > 0)
		Writing::render(text, rs);
	else
		Writing::render(placeholder, rs);
	rs.popTransform();
	rs.popTransform();

	Window::render(rs);

	t += 0.025f;

	if (t > 2.0f)
		t -= 2.0f;

	if (t < 1.0f && focus)
	{
		rs.pushTransform();

		rs.addTransform(Matrix4::Translation(Writing::getAdvance(text.substr(0, caret))));

		if (image)
			image->render(rs, Vec2(4.0f, size.y));

		rs.popTransform();
	}
}

#include "PlatformEvents.h"

bool TextBoxWindow::handleEvent(IEvent * pEvent)
{
	if (pEvent->GetType()==KeyDownEvent::event_type)
	{
		KeyDownEvent * ev = (KeyDownEvent*)pEvent;
		bool action = false;
		if (ev->key==Platform::KeyEvent::LMB)
		{
			caret = (ev->x - min.x) / size.x;
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
	return Window::handleEvent(pEvent);
}