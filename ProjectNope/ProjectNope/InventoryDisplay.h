#ifndef INVENTORY_DISPLAY_H
#define INVENTORY_DISPLAY_H

#include "Inventory.h"

class InventoryDisplay
{
public:
	InventoryDisplay(void);
	~InventoryDisplay(void);

	void update(Inventory * const inventory);

	Inventory * last;
	int sync;
	std::vector<std::shared_ptr<Item>> display_list;
};

#endif