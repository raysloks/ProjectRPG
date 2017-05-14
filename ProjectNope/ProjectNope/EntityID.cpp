#include "EntityID.h"

EntityID::EntityID(void)
{
	id = 0xffffffff;
	uid = 0;
}

EntityID::EntityID(uint32_t a, uint32_t b)
{
	id = a;
	uid = b;
}

outstream& operator<<(outstream& os, const EntityID& id)
{
	os << id.id << id.uid;
	return os;
}

instream& operator>>(instream& is, EntityID& id)
{
	is >> id.id >> id.uid;
	return is;
}
