#ifndef STANDARD_HUD_H
#define STANDARD_HUD_H

#include "RectangleWindow.h"

class World;
class Client;

class StandardHUD
	: public RectangleWindow
{
public:
	StandardHUD(World * world, Client * client, int x, int y, int width, int height);
	~StandardHUD(void);

	void render(void);

	bool handleEvent(IEvent * pEvent);
private:
	World * world;
	Client * client;

	float last_filled;
};

#endif