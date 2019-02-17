#pragma once

#include "Decorator.h"

class Item
{
public:
	Item();
	~Item();

	void writeLog(outstream& os) const;
	void readLog(instream& is);

	uint32_t type;
};

outstream& operator<<(outstream& os, const Item& item);
instream& operator>>(instream& is, Item& item);