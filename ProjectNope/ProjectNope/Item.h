#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <memory>

#include "streams.h"

class ScriptMemory;

class Item
{
public:
	Item(void);
	~Item(void);

	void writeLog(outstream& os);
	void readLog(instream& is);

	void render(void);

	size_t dec;

	std::string name;

	std::string _script;
	std::shared_ptr<ScriptMemory> mem;
};

#endif