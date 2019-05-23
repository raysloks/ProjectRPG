#pragma once

#include "Decorator.h"

#include "GlobalPosition.h"

class NewEntity;
class World;

class Item
{
public:
	Item();
	Item(uint32_t type);
	~Item();

	void writeLog(outstream& os) const;
	void readLog(instream& is);

	static NewEntity * drop(World * world, uint32_t type, const GlobalPosition& pos);

	uint32_t type;
};

outstream& operator<<(outstream& os, const Item& item);
instream& operator>>(instream& is, Item& item);