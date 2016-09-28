#ifndef CLIENT_EVENTS_H
#define CLIENT_EVENTS_H

#include "IEvent.h"

class ActiveEvent :
	public IEvent
{
public:
	ActiveEvent(void);
	~ActiveEvent(void);

	const EventType& GetType(void) const;

	static const EventType event_type;
};

#endif