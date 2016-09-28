#include "ClientEvents.h"

const EventType ActiveEvent::event_type("ActiveEvent");

ActiveEvent::ActiveEvent(void)
{
}

ActiveEvent::~ActiveEvent(void)
{
}

const EventType& ActiveEvent::GetType(void) const
{
	return event_type;
}
