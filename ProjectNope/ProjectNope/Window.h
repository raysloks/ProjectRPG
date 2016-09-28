#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include "IEvent.h"
#include <boost\function.hpp>

class Window
{
public:
	Window(void);
	Window(Window * parent);
	virtual ~Window(void);

	virtual void render(void);

	bool addChild(Window * child);

	virtual bool handleEvent(IEvent * pEvent);

	boost::function<void(void)> onClick, onRender;
	
protected:
	Window * parent;
	std::vector<Window*> children;
};

#endif