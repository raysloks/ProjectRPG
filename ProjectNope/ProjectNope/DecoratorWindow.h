#pragma once

#include "Window.h"

#include "Decorator.h"

class DecoratorWindow :
	public Window
{
public:
	DecoratorWindow();
	~DecoratorWindow();

	void render(RenderSetup& rs);

	std::shared_ptr<Decorator> dec;
};

