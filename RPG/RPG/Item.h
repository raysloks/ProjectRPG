#ifndef ITEM_H
#define ITEM_H

#include "Decorator.h"

class Item
{
public:
	Item();
	~Item();

	void writeLog(outstream& os) const;
	void readLog(instream& is);

	std::string name, desc, icon;
	std::shared_ptr<Decorator> dec;
};

#endif