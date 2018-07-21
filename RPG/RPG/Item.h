#ifndef ITEM_H
#define ITEM_H

#include "Decorator.h"

class Item
{
public:
	Item();
	~Item();

	std::string name;
	std::shared_ptr<Decorator> dec;
};

#endif