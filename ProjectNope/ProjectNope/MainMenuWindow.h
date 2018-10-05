#ifndef MAIN_MENU_WINDOW_H
#define MAIN_MENU_WINDOW_H

#include "RectangleWindow.h"

class World;
class Client;

class MainMenuWindow
	: public RectangleWindow
{
public:
	MainMenuWindow(World * world, Client * client, int x, int y, int width, int height);
	~MainMenuWindow(void);

	void render(RenderSetup& rs);

	bool handleEvent(IEvent * pEvent);
//private:
	World * world;
	Client * client;

	int state;
	float time, time_move;
};

#endif