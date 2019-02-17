#pragma once

#include "Decorator.h"

class ItemType
{
public:
	ItemType();
	~ItemType();

	static void init();
	static ItemType * get(uint32_t index);
	static void release();

	std::string name, desc, icon;
	std::shared_ptr<Decorator> dec;
};