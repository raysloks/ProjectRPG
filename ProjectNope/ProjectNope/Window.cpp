#include "Window.h"
#include <algorithm>

Window::Window(void)
{
	parent = 0;
}

Window::Window(Window * pParent)
{
	pParent->addChild(this);
}

Window::~Window(void)
{
	if (parent != nullptr)
	{
		parent->children.erase(std::remove(parent->children.begin(), parent->children.end(), this), parent->children.end());
	}
}

void Window::render(RenderSetup& rs)
{
	for (auto child : children)
	{
		if (child->onRender)
			child->onRender();
		child->render(rs);
	}
}

bool Window::addChild(Window * child)
{
	if (child->parent != nullptr)
	{
		child->parent->children.erase(std::remove(child->parent->children.begin(), child->parent->children.end(), child), child->parent->children.end());
	}
	child->parent = this;
	children.push_back(child);
	return true;
}

bool Window::handleEvent(IEvent * pEvent)
{
	for (auto child : children)
	{
		if (child->handleEvent(pEvent))
			return true;
	}
	return false;
}