#include "IEventManager.h"
#include "IPlatform.h"
#include "GUIObject.h"
#include "PlatformEvents.h"

#ifndef TEMP_PLATFORM_H
#define TEMP_PLATFORM_H

class TempPlatform : public IPlatform
{
public:
	TempPlatform(void);
	~TempPlatform(void);

	void apply(void);

	void set_z_depth(int depth);
	void set_fullscreen(bool fullscreen, int monitor);
	void set_position(int x, int y);
	void resize(int w, int h);

	int get_width(void);
	int get_height(void);

	void input(IEventManager * pEventManager, bool lock_cursor, bool hide_cursor);
	void swap(void);
	void set_vsync(bool on);
	bool get_vsync(void);

	void set_cursor_position(int x, int y);

	bool has_focus(void);

	void release(void);
private:
	Platform::GUIObject * ptr;

	int mX, mY, mW, mH, z_buffer_depth;
	bool use_fullscreen, use_vsync;
	int fullscreen_monitor;
};

#endif