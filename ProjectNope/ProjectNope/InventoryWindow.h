#ifndef INVENTORY_WINDOW_H
#define INVENTORY_WINDOW_H

#include "RectangleWindow.h"
#include "InventoryDisplay.h"

class World;
class Client;

class InventoryWindow :
	public RectangleWindow
{
public:
	InventoryWindow(World * world, Client * client);
	~InventoryWindow(void);

	void render(void);

	bool handleEvent(IEvent * pEvent);
	
	World * world;
	Client * client;

	InventoryDisplay disp;

	int state;
};

#endif