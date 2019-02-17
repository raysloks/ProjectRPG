#ifndef PLATFORM_EVENTS_H
#define PLATFORM_EVENTS_H

#include "IEvent.h"
#include "Vec2.h"

class KeyDownEvent :
	public IEvent
{
public:
	int key;
	std::string chr;
	union
	{
		Vec2 p;
		struct 
		{
			float x, y;
		};
	};

	KeyDownEvent(int key_code, const std::string& c);
	~KeyDownEvent(void);

	const EventType& GetType(void) const;

	static const EventType event_type;
};

class KeyUpEvent :
	public IEvent
{
public:
	int key;
	std::string chr;
	union
	{
		Vec2 p;
		struct
		{
			float x, y;
		};
	};

	KeyUpEvent(int key_code, const std::string& c);
	~KeyUpEvent(void);

	const EventType& GetType(void) const;

	static const EventType event_type;
};

class MouseMoveEvent :
	public IEvent
{
public:
	union
	{
		Vec2 p;
		struct
		{
			float x, y;
		};
	};
	bool relative;

	MouseMoveEvent(int a, int b, bool c);
	~MouseMoveEvent(void);

	const EventType& GetType(void) const;

	static const EventType event_type;
};

class MouseWheelEvent :
	public IEvent
{
public:
	int d;

	MouseWheelEvent(int d);
	~MouseWheelEvent(void);

	const EventType& GetType(void) const;

	static const EventType event_type;
};

class PlatformDestroyEvent :
	public IEvent
{
public:
	bool stop;

	PlatformDestroyEvent(void);
	~PlatformDestroyEvent(void);

	const EventType& GetType(void) const;

	static const EventType event_type;
};

class ResizeEvent :
	public IEvent
{
public:
	int w, h;

	ResizeEvent(int width, int height);
	~ResizeEvent(void);

	const EventType& GetType(void) const;

	static const EventType event_type;
};

#endif