#ifndef TEXT_BOX_WINDOW_H
#define TEXT_BOX_WINDOW_H

#include "TextWindow.h"

class TextBoxWindow :
	public TextWindow
{
public:
	TextBoxWindow(int x, int y, int w, int h);
	~TextBoxWindow(void);

	std::string placeholder;
	size_t caret;
	float t;

	void render(void);

	bool handleEvent(IEvent * pEvent);
};

#endif