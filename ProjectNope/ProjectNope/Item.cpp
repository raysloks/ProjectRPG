#include "Item.h"
#include "GUIObject.h"
#include "Writing.h"

Item::Item(void)
{
}

Item::~Item(void)
{
}

void Item::writeLog(outstream& os)
{
	os << name;
}

void Item::readLog(instream& is)
{
	is >> name;
}

void Item::render(void) //TODO move to ItemRender or InventoryRenderer or something
{
	//Writing::render(name);
	glTranslatef(0.0f, 16.0f, 0.0f);
}