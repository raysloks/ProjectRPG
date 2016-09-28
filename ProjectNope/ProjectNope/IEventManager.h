#include <string>
#include "IEvent.h"

#ifndef INTERFACE_EVENT_MANAGER_H
#define INTERFACE_EVENT_MANAGER_H

class IEventListener;

class IEventManager
{
public:
	virtual void AddListener(IEventListener * pListener, const EventType& eventType) = 0;
	virtual void RemoveListener(IEventListener * pListener, const EventType& eventType) = 0;

	virtual void Trigger(IEvent * pEvent) = 0; // this one will NOT delete pEvent!
	virtual void QueueEvent(IEvent * pEvent) = 0; // this one WILL!

	virtual void Tick(void) = 0;
};

#endif