#ifndef ENTITY_ID_H
#define ENTITY_ID_H

#include "streams.h"

class EntityID
{
public:
	EntityID(void);
	EntityID(uint32_t id, uint32_t uid);

	uint32_t id, uid;

	bool operator==(const EntityID& rhs) const;
	bool operator!=(const EntityID& rhs) const;
};

#endif