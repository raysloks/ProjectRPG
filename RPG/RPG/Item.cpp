#include "Item.h"

Item::Item()
{
	type = 0;
}

Item::~Item()
{
}

void Item::writeLog(outstream& os) const
{
	os << type;
}

void Item::readLog(instream& is)
{
	is >> type;
}

outstream& operator<<(outstream& os, const Item& item)
{
	item.writeLog(os);
	return os;
}

instream& operator>>(instream& is, Item& item)
{
	item.readLog(is);
	return is;
}