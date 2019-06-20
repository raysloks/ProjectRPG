#include "TempPlatform.h"
#include <iostream>

class TPWindow :
	public Platform::GUIObject
{
public:
	TPWindow(const std::string& name, int x, int y, int w, int h, int z_depth, bool fullscreen, int fullscreen_monitor, TempPlatform * pP) :
		GUIObject(name, x, y, w, h, z_depth, fullscreen, fullscreen_monitor), platform(pP) {}
	
	IEventManager * pCallback;
private:
	void OnKeyDown(const Platform::KeyEvent& key) {
		KeyDownEvent * ev = new KeyDownEvent(key.code, key.chr);
		ev->x = mouse_x;
		ev->y = mouse_y;
		pCallback->QueueEvent(ev);
	}
	void OnKeyUp(const Platform::KeyEvent& key) {
		KeyUpEvent * ev = new KeyUpEvent(key.code, key.chr);
		ev->x = mouse_x;
		ev->y = mouse_y;
		pCallback->QueueEvent(ev);
	}
	void OnMouseMove(int x, int y, bool relative) {
		MouseMoveEvent * ev = new MouseMoveEvent(x, y, relative);
		if (!relative) {
			mouse_x = x;
			mouse_y = y;
		}
		pCallback->QueueEvent(ev);
	}
	void OnMouseWheel(int d) {
		MouseWheelEvent * ev = new MouseWheelEvent(d);
		pCallback->QueueEvent(ev);
	}
	void OnResize(void) {
		glViewport(0, 0, GetWidth(), GetHeight());
		ResizeEvent * ev = new ResizeEvent(GetWidth(), GetHeight());
		pCallback->QueueEvent(ev);
		platform->resize(GetWidth(), GetHeight());
	}

	bool OnDestroy(void) {
		PlatformDestroyEvent * ev = new PlatformDestroyEvent();
		pCallback->Trigger(ev);
		bool stopped = ev->stop;
		delete ev;
		return stopped;
	}

	int mouse_x, mouse_y;

	TempPlatform * platform;
};

TempPlatform::TempPlatform(void)
{
	ptr = 0;
	use_fullscreen = false;
	z_buffer_depth = 0;
	mX = 32;
	mY = 32;
	mW = 640;
	mH = 480;
	lock = true;
}

TempPlatform::~TempPlatform(void)
{
}

void TempPlatform::apply(void)
{
	if (ptr!=0) {
		ptr->Destroy();
		delete ptr;
	}
	ptr = new TPWindow("RPG", mX, mY, mW, mH, z_buffer_depth, use_fullscreen, fullscreen_monitor, this);
	ptr->getContext()->SetVSync(use_vsync);
}

void TempPlatform::set_z_depth(int depth)
{
	z_buffer_depth = depth;
}

void TempPlatform::set_fullscreen(bool fullscreen, int monitor)
{
	use_fullscreen = fullscreen;
	fullscreen_monitor = monitor;
}

void TempPlatform::set_position(int x, int y)
{
	mX = x;
	mY = y;
}

void TempPlatform::resize(int w, int h)
{
	mW = w;
	mH = h;
}

int TempPlatform::get_width(void)
{
	return ptr->GetWidth();
}

int TempPlatform::get_height(void)
{
	return ptr->GetHeight();
}

void TempPlatform::input(IEventManager * pEventManager, bool lock_cursor, bool hide_cursor)
{
	if (ptr)
	{
		((TPWindow*)ptr)->pCallback = pEventManager;
		lock = lock_cursor;
		hide = hide_cursor;
		update_cursor_lock_and_hide();
		ptr->Tick();
	}
	Platform::GUIObject::PollControllerState();
}

void TempPlatform::input(IEventManager * pEventManager)
{
	if (ptr)
	{
		((TPWindow*)ptr)->pCallback = pEventManager;
		ptr->Tick();
	}
	Platform::GUIObject::PollControllerState();
}

void TempPlatform::swap(void)
{
	if (ptr!=0) {
		ptr->getContext()->Swap();
	}
}

void TempPlatform::set_vsync(bool on)
{
	if (on!=use_vsync)
		if (ptr!=0)
			ptr->getContext()->SetVSync(on);
	use_vsync = on;
}

bool TempPlatform::get_vsync(void)
{
	return use_vsync;
}

void TempPlatform::set_cursor_position(int x, int y)
{
	if (ptr)
		ptr->SetCursorPosition(ptr->GetX() + x, ptr->GetY() + y);
}

void TempPlatform::set_cursor_lock(bool lock_cursor)
{
	lock = lock_cursor;
	update_cursor_lock_and_hide();
}

void TempPlatform::set_cursor_hide(bool hide_cursor)
{
	hide = hide_cursor;
	update_cursor_lock_and_hide();
}

bool TempPlatform::has_focus(void)
{
	if (ptr!=0)
		return ptr->HasFocus();
	return false;
}

void TempPlatform::release(void)
{
	if (ptr!=0) {
		ptr->Destroy();
		delete ptr;
		ptr = 0;
	}
}

void TempPlatform::update_cursor_lock_and_hide()
{
	if (ptr->HasFocus()/* && lock*/) {
		ptr->SetCursorVisible(!hide);
		ptr->SetCursorLock(true);
	}
	else {
		ptr->SetCursorVisible(true);
		ptr->SetCursorLock(false);
	}
}
