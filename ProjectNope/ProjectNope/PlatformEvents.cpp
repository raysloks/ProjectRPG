#include "PlatformEvents.h"

const EventType KeyDownEvent::event_type("KeyDownEvent");

const EventType KeyUpEvent::event_type("KeyUpEvent");

const EventType MouseMoveEvent::event_type("MouseMoveEvent");

const EventType MouseWheelEvent::event_type("MouseWheelEvent");

const EventType PlatformDestroyEvent::event_type("PlatformDestroyEvent");

const EventType ResizeEvent::event_type("ResizeEvent");

const EventType FocusLostEvent::event_type("FocusLostEvent");

KeyDownEvent::KeyDownEvent(int key_code, const std::string& c) : key(key_code), chr(c)
{
}

KeyUpEvent::KeyUpEvent(int key_code, const std::string& c) : key(key_code), chr(c)
{
}

MouseMoveEvent::MouseMoveEvent(float a, float b, bool c) : x(a), y(b), relative(c)
{
}

MouseWheelEvent::MouseWheelEvent(int delta) : d(delta)
{
}

PlatformDestroyEvent::PlatformDestroyEvent(void) : stop(false)
{
}

ResizeEvent::ResizeEvent(float a, float b) : w(a), h(b)
{
}

KeyDownEvent::~KeyDownEvent(void)
{
}

KeyUpEvent::~KeyUpEvent(void)
{
}

MouseMoveEvent::~MouseMoveEvent(void)
{
}

MouseWheelEvent::~MouseWheelEvent(void)
{
}

PlatformDestroyEvent::~PlatformDestroyEvent(void)
{
}

ResizeEvent::~ResizeEvent(void)
{
}

const EventType& KeyDownEvent::GetType(void) const
{
	return event_type;
}

const EventType& KeyUpEvent::GetType(void) const
{
	return event_type;
}

const EventType& MouseMoveEvent::GetType(void) const
{
	return event_type;
}

const EventType& MouseWheelEvent::GetType(void) const
{
	return event_type;
}

const EventType& PlatformDestroyEvent::GetType(void) const
{
	return event_type;
}

const EventType& ResizeEvent::GetType(void) const
{
	return event_type;
}

const EventType& FocusLostEvent::GetType(void) const
{
	return event_type;
}