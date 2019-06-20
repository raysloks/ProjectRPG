#include "Window.h"

#include <algorithm>

#include <boost/range/adaptor/reversed.hpp>

#include "RenderSetup.h"

Window::Window(void)
{
	parent = nullptr;
	focus = nullptr;
}

Window::Window(Window * pParent)
{
	parent = nullptr;
	focus = nullptr;
	pParent->addChild(this);
}

Window::~Window(void)
{
	for (auto child : children)
	{
		child->parent = nullptr;
		delete child;
	}
	if (parent != nullptr)
	{
		parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
	}
}

void Window::render(RenderSetup& rs)
{
	if (onRender)
		onRender();
	rs.pushTransform();
	rs.addTransform(Matrix4::Translation(min));
	for (auto child : children)
		child->render(rs);
	rs.popTransform();
}

void Window::addChild(Window * child)
{
	if (child->parent)
		child->parent->removeChild(child);
	child->parent = this;
	children.push_back(child);
}

void Window::removeChild(Window * child)
{
	if (focus == child)
		focus = nullptr;
	children.erase(std::find(children.begin(), children.end(), child));
	child->parent = nullptr;
}

#include "PlatformEvents.h"
#include "KeyEvent.h"

bool Window::handleEvent(IEvent * pEvent)
{
	IEvent* pass_on = nullptr;
	bool click = false;
	bool only_focus = false;
	if (pEvent->GetType() == KeyDownEvent::event_type)
	{
		KeyDownEvent* ev = (KeyDownEvent*)pEvent;

		if (ev->key == Platform::KeyEvent::LMB || ev->key == Platform::KeyEvent::MMB || ev->key == Platform::KeyEvent::RMB)
		{
			if (ev->x >= min.x && ev->y >= min.y && ev->x < max.x && ev->y < max.y)
			{
				click = true;
				if (parent)
					parent->focus = this;
			}
		}
		else
		{
			only_focus = true;
		}

		pass_on = new KeyDownEvent(*ev);
		((KeyDownEvent*)pass_on)->x -= min.x;
		((KeyDownEvent*)pass_on)->y -= min.y;
	}
	if (pEvent->GetType() == KeyUpEvent::event_type)
	{
		KeyUpEvent* ev = (KeyUpEvent*)pEvent;
		pass_on = new KeyUpEvent(*ev);
		((KeyUpEvent*)pass_on)->x -= min.x;
		((KeyUpEvent*)pass_on)->y -= min.y;
	}
	if (pEvent->GetType() == MouseMoveEvent::event_type)
	{
		MouseMoveEvent* ev = (MouseMoveEvent*)pEvent;
		pass_on = new MouseMoveEvent(*ev);
		if (!ev->relative)
		{
			((MouseMoveEvent*)pass_on)->x -= min.x;
			((MouseMoveEvent*)pass_on)->y -= min.y;
		}
	}
	if (pEvent->GetType() == ResizeEvent::event_type)
	{
		ResizeEvent* ev = (ResizeEvent*)pEvent;

		min = ev->size * min_anchor + min_offset;
		max = ev->size * max_anchor + max_offset;
		size = max - min;

		pass_on = new ResizeEvent(*ev);
		((ResizeEvent*)pass_on)->size = size;
	}
	if (pass_on)
	{
		if (only_focus)
		{
			if (focus)
				focus->handleEvent(pass_on);
		}
		else
		{
			for (auto child : boost::adaptors::reverse(children))
			{
				if (child->handleEvent(pass_on))
					break;
			}
		}
	}
	if (click)
		if (onClick)
			onClick();
	return click;
}