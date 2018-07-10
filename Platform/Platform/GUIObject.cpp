#include "GUIObject.h"

namespace Platform
{
	static bool isInit = false; // TODO // add support for error codes
	static WNDCLASSEX wcMold;
	static HINSTANCE hInstance;
	static const char * wc_name = "WindowMold";

	static bool _cvis;

	int GUIObject::pollFrequency = 60;
	XINPUT_STATE GUIObject::controllerState[4];

	LRESULT CALLBACK GUIObject::WinProc(HWND hWnd,
		UINT Msg,
		WPARAM wParam,
		LPARAM lParam)
	{
		GUIObject * obj = (GUIObject*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (obj==0)
			return DefWindowProc(hWnd, Msg, wParam, lParam);
		switch (Msg)
		{
		case WM_CLOSE:
			return obj->OnDestroy();
		case WM_MOVE:
			obj->mX = LOWORD(lParam);
			obj->mY = HIWORD(lParam);
			if (obj->crsLock)
			{
				RECT rect = {obj->mX, obj->mY, obj->mX + obj->mW, obj->mY + obj->mH};
				ClipCursor(&rect);
			}
			return 0;
		case WM_SIZE:
			obj->mW = LOWORD(lParam);
			obj->mH = HIWORD(lParam);
			obj->OnResize();
			if (obj->crsLock)
			{
				RECT rect = {obj->mX, obj->mY, obj->mX + obj->mW, obj->mY + obj->mH};
				ClipCursor(&rect);
			}
			return 0;
		case WM_KEYDOWN:
			//obj->OnKeyDown(KeyEvent(wParam, lParam, true));
			return 0;
		case WM_KEYUP:
			//obj->OnKeyUp(KeyEvent(wParam, lParam, false));
			return 0;
		case WM_LBUTTONDOWN: obj->OnKeyDown((int)KeyEvent::LMB); return 0;
		case WM_MBUTTONDOWN: obj->OnKeyDown((int)KeyEvent::MMB); return 0;
		case WM_RBUTTONDOWN: obj->OnKeyDown((int)KeyEvent::RMB); return 0;
		case WM_LBUTTONUP: obj->OnKeyUp((int)KeyEvent::LMB); return 0;
		case WM_MBUTTONUP: obj->OnKeyUp((int)KeyEvent::MMB); return 0;
		case WM_RBUTTONUP: obj->OnKeyUp((int)KeyEvent::RMB); return 0;
		case WM_MOUSEMOVE:
			obj->OnMouseMove(LOWORD(lParam), HIWORD(lParam), false);
			return 0;
		case WM_INPUT:
			{
				UINT dwSize;
				GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, 
					sizeof(RAWINPUTHEADER));
				LPBYTE lpb = new BYTE[dwSize];
				if (lpb == NULL)
				{
					return 0;
				}
				GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));

				RAWINPUT* raw = (RAWINPUT*)lpb;

				if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					obj->OnMouseMove(raw->data.mouse.lLastX, raw->data.mouse.lLastY, true);
					if (raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL)
						obj->OnMouseWheel(static_cast<signed short>(raw->data.mouse.usButtonData));
				}

				if (raw->header.dwType == RIM_TYPEKEYBOARD)
				{
					unsigned short key = raw->data.keyboard.VKey;
					bool extended = (raw->data.keyboard.Flags & 0x2) != 0;
					if (key==VK_SHIFT) key=extended ? VK_RSHIFT : VK_LSHIFT;
					if (key==VK_CONTROL) key=extended ? VK_RCONTROL : VK_LCONTROL;
					if (raw->data.keyboard.Flags & 0x1) {
						obj->OnKeyUp(KeyEvent(key));
					} else {
						obj->OnKeyDown(KeyEvent(key));
					}
				}

				delete[] lpb;
				return 0;
			}
		default:
			break;
		}
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	void GUIObject::_InitWnd(void)
	{
		if (!isInit)
		{
			hInstance = GetModuleHandle( 0 );

			ZeroMemory(&wcMold, sizeof(WNDCLASSEX));
			wcMold.cbSize = sizeof(WNDCLASSEX);
			wcMold.style = CS_OWNDC;
			wcMold.lpfnWndProc = GUIObject::WinProc;
			wcMold.hInstance = hInstance;
			wcMold.hCursor = LoadCursor(NULL, IDC_ARROW);
			//wcMold.hbrBackground = (HBRUSH)2;
			wcMold.lpszMenuName = NULL;
			wcMold.lpszClassName = wc_name;
			RegisterClassEx(&wcMold);

			_cvis = true;
			isInit = true;

			RAWINPUTDEVICE rid[2];
			rid[0].usUsagePage = 0x01;
			rid[0].usUsage = 0x02;
			rid[0].dwFlags = 0;
			rid[0].hwndTarget = 0;

			rid[1].usUsagePage = 0x01;
			rid[1].usUsage = 0x06;
			rid[1].dwFlags = 0;
			rid[1].hwndTarget = 0;

			RegisterRawInputDevices(rid, 2, sizeof(rid[0]));
		}

		DWORD dwExStyle, dwStyle;

		if (isFullScr) {
			DEVMODE dmScreenSettings;
			memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
			dmScreenSettings.dmSize=sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth    = mW;
			dmScreenSettings.dmPelsHeight   = mH;
			dmScreenSettings.dmBitsPerPel   = 32;
			dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			mX = 0;
			mY = 0;

			dwExStyle = WS_EX_APPWINDOW;
			dwStyle = WS_POPUP;
		} else {
			//dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwExStyle = 0;
			//dwStyle = WS_POPUP;
			dwStyle = WS_OVERLAPPEDWINDOW;
		}

		RECT wr = {mX, mY, mX + mW, mY + mH};
		AdjustWindowRectEx(&wr, dwStyle, FALSE, dwExStyle);

		hWnd = CreateWindowEx(dwExStyle, wc_name, mName.c_str(),
			dwStyle, wr.left, wr.top,
			wr.right - wr.left, wr.bottom - wr.top,
			NULL, NULL,
			hInstance, NULL);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);
		pContext = std::shared_ptr<RenderContext>(new RenderContext(hWnd, z_depth));
		ShowWindow(hWnd, SW_SHOW);
		SetFocus(hWnd);
		SetForegroundWindow(hWnd);
	}

	GUIObject::GUIObject(const std::string& name, int x, int y, int w, int h, int depth_depth, bool fullscreen)
	{
		mName = name;
		mX = x;
		mY = y;
		mW = w;
		mH = h;
		z_depth = depth_depth;
		isFullScr = fullscreen;
		crsLock = false;
		_InitWnd();
	}

	static MSG msg;

	void GUIObject::Tick(void)
	{
		while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void GUIObject::PollControllerState(void)
	{
		DWORD dwResult;
		for (int i=0;i<3;++i)
		{
			ZeroMemory(&controllerState[i], sizeof(XINPUT_STATE));

			dwResult = XInputGetState(0, &controllerState[i]);

			if (dwResult == ERROR_SUCCESS)
			{
			}
			else
			{
			}
		}
	}

	void GUIObject::SetCursorVisible(bool visible) { if (_cvis!=visible) ShowCursor(visible); _cvis = visible; }
	void GUIObject::SetCursorPosition(int x, int y) { SetCursorPos(x, y); }
	bool GUIObject::HasCursor(void) {
		//POINT point;
		//GetCursorPos(&point);
		//return point.x>=mX && point.y>=mY && point.x<=mX+mW && point.y<=mY+mH && GetFocus()==hWnd && GetForegroundWindow()==hWnd;
		return GetForegroundWindow()==hWnd;
	}

	GUIObject::~GUIObject(void)
	{
		SetWindowLongPtr(hWnd, GWLP_USERDATA, NULL);
	}

	void GUIObject::Destroy(void)
	{
		DestroyWindow(hWnd);
	}

	void GUIObject::SetCursorLock(bool locked)
	{
		if (locked)
		{
			RECT rect = {mX, mY, mX + mW, mY + mH};
			ClipCursor(&rect);
		} else if (crsLock) {
			ClipCursor(0);
		}
		crsLock = locked;
	}
}