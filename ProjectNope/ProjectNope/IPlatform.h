#ifndef INTERFACE_PLATFORM_H
#define INTERFACE_PLATFORM_H

class IEventManager;

class IPlatform
{
public:
	virtual void apply(void) = 0;

	virtual void set_z_depth(int depth) = 0;
	virtual void set_fullscreen(bool fullscreen) = 0;
	virtual void set_position(int x, int y) = 0;
	virtual void resize(int w, int h) = 0;

	virtual int get_width(void) = 0;
	virtual int get_height(void) = 0;

	virtual void input(IEventManager * pEventManager, bool lock_cursor, bool hide_cursor) = 0;
	virtual void swap(void) = 0;
	virtual void set_vsync(bool on) = 0;
	virtual bool get_vsync(void) = 0;

	virtual bool has_focus(void) = 0;

	virtual void release(void) = 0;
};

#endif