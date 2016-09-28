#ifndef INVENTORY_H
#define INVENTORY_H

#include "SyncContainer.h"
#include "Item.h"

typedef SyncContainer<Item> Inventory;
//TODO when in need of new inventory functionality, make new inheriting class or something

#endif