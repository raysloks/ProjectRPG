#include "HashedString.h"

#ifndef INTERFACE_EVENT_H
#define INTERFACE_EVENT_H

typedef HashedString EventType;

class IEvent
{
public:
	virtual ~IEvent(void) = 0;

	virtual const EventType& GetType(void) const = 0;
};

#endif