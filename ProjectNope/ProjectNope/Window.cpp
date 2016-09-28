#include "Window.h"

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
	if (parent!=0) {
		for (std::vector<Window*>::iterator it=children.begin();it!=children.end();++it) {
			if (*it==this)
				parent->children.erase(it);
		}
	}
}

void Window::render(void)
{
	for (std::vector<Window*>::iterator it=children.begin();it!=children.end();++it)
	{
		if (!(*it)->onRender.empty())
			(*it)->onRender();
		(*it)->render();
	}
}

bool Window::addChild(Window * child)
{
	if (child->parent!=0) {
		for (std::vector<Window*>::iterator it=child->parent->children.begin();it!=child->parent->children.end();++it) {
			if (it[0]==child)
				child->parent->children.erase(it);
		}
	}
	child->parent = this;
	children.push_back(child);
	return true;
}

bool Window::handleEvent(IEvent * pEvent)
{
	for (std::vector<Window*>::iterator it=children.begin();it!=children.end();++it)
	{
		if ((*it)->handleEvent(pEvent))
			return true;
	}
	return false;
}