#ifndef WINDOW_H
#define WINDOW_H

#include <vector>
#include "IEvent.h"
#include <functional>

#include "Vec2.h"

class RenderSetup;

class Window
{
public:
	Window(void);
	Window(Window * parent);
	virtual ~Window(void);

	virtual void render(RenderSetup& rs);

	void addChild(Window* child);
	void removeChild(Window* child);

	virtual bool handleEvent(IEvent * pEvent);

	std::function<void(void)> onClick, onRender;

	Vec2 min_anchor, max_anchor;
	Vec2 min_offset, max_offset;

	Vec2 min, max, size;
	
protected:
	Window * parent;
	Window * focus;
	std::vector<Window*> children;
};

#endif