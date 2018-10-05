#ifndef JOIN_GAME_WINDOW_H
#define JOIN_GAME_WINDOW_H

#include "RectangleWindow.h"

class World;
class Client;

class JoinGameWindow :
	public RectangleWindow
{
public:
	JoinGameWindow(World * world, Client * client, int x, int y, int width, int height);
	~JoinGameWindow(void);

	void render(RenderSetup& rs);

	bool handleEvent(IEvent * pEvent);
	//private:
	World * world;
	Client * client;

	int state;
};

#endif