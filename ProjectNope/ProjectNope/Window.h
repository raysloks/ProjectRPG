#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include "IEvent.h"
#include <functional>

class RenderSetup;

class Window
{
public:
	Window(void);
	Window(Window * parent);
	virtual ~Window(void);

	virtual void render(RenderSetup& rs);

	bool addChild(Window * child);

	virtual bool handleEvent(IEvent * pEvent);

	std::function<void(void)> onClick, onRender;
	
protected:
	Window * parent;
	std::vector<Window*> children;
};

#endif