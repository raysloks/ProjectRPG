#include "IEvent.h"

#ifndef INTERFACE_EVENT_LISTENER_H
#define INTERFACE_EVENT_LISTENER_H

class IEventListener
{
public:
	virtual bool HandleEvent(IEvent * pEvent) = 0;
};

#endif