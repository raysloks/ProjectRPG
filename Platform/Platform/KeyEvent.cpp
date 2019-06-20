#include "KeyEvent.h"
#include <iostream>

namespace Platform
{

	KeyEvent::KeyEvent(unsigned short key)
	{
		LPWSTR wbuf = new WCHAR[8];
		PBYTE state = new BYTE[256];
		GetKeyboardState(state);
		int wi = ToUnicode(key, 0, state, wbuf, 8, 0);
		char * buf = new char[256];
		int mbi = WideCharToMultiByte(CP_UTF8, 0, wbuf, wi, buf, 256, 0, 0);
		for (int i=0;i<mbi;++i)
			chr.push_back(buf[i]);

		delete [] buf;
		delete [] state;
		delete [] wbuf;

		if (key > 0x0040 && key < 0x005b) {
			code = key-0x41; return; }
		if (key >= 0x0025 && key <= 0x0028) {
			code = key; return; }
		if (key == VK_LSHIFT) {
			code = LSHIFT; return; }
		if (key == VK_RSHIFT) {
			code = RSHIFT; return; }
		if (key == VK_LCONTROL) {
			code = LCTRL; return; }
		if (key == VK_RCONTROL) {
			code = RCTRL; return; }
		if (key == VK_ESCAPE) { code = ESC; return; }
		if (key == VK_SPACE) { code = SPACE; return; }
		if (key == VK_DELETE) { code = DEL; return; }
		if (key == VK_RETURN) { code = ENTER; return; }
		if (key == VK_TAB) { code = TAB; return; }
		if (key >= VK_F1 && key <= VK_F24) { code = key - 0x3f; return; }
		if (key >= 0x30 && key <= 0x39) { code = key + 0x20; return; }
		code = 0xffffffff;
	}

	KeyEvent::KeyEvent(int key)
	{
		code = key;
	}

	KeyEvent::~KeyEvent(void)
	{
	}

}