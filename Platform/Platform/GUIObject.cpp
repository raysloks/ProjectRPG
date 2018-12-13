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
				if (key == VK_SHIFT) key = extended ? VK_RSHIFT : VK_LSHIFT;
				if (key == VK_CONTROL) key = extended ? VK_RCONTROL : VK_LCONTROL;
				if (raw->data.keyboard.Flags & 0x1) {
					obj->OnKeyUp(KeyEvent(key));
				}
				else {
					obj->OnKeyDown(KeyEvent(key));
				}
			}

			delete[] lpb;
			return 0;
		}
		case WM_SETCURSOR:
		{

			break;
		}
		case WM_SETFOCUS:
		{
			obj->SetCursorLock(obj->crsLock);
			return 0;
		}
		case WM_KILLFOCUS:
		{
			ClipCursor(0);
			return 0;
		}
		default:
			break;
		}
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	struct DisplayInfo
	{
		RECT rect;
		std::string name;
	};

	std::vector<DisplayInfo> monitors;

	BOOL MyMonitorEnumProc(HMONITOR hMonitor, HDC hDC, LPRECT Arg3, LPARAM Arg4)
	{
		MONITORINFOEX monitor_info;
		monitor_info.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &monitor_info);

		DISPLAY_DEVICE dd;
		dd.cb = sizeof(dd);
		int monitorIndex = 0;
		while (EnumDisplayDevices(monitor_info.szDevice, monitorIndex, &dd, 0))
		{
			monitors.push_back({ monitor_info.rcMonitor, dd.DeviceString });
			std::cout << dd.DeviceName << ", " << dd.DeviceString << "\n";
			++monitorIndex;
		}

		return TRUE;
	}

	void UpdateMonitorInfo()
	{
		monitors.clear();
		EnumDisplayMonitors(NULL, NULL, MyMonitorEnumProc, NULL);
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
			wcMold.hCursor = (HICON)LoadImage(NULL, "cursor.ico", 1, 64, 64, LR_LOADFROMFILE);
			//wcMold.hbrBackground = (HBRUSH)2;
			wcMold.lpszMenuName = NULL;
			wcMold.lpszClassName = wc_name;
			RegisterClassEx(&wcMold);

			HWND dummy = CreateWindowEx(0, wc_name, "dummy", 0, 0, 0, 0, 0, 0, 0, hInstance, 0);
			RenderContext::LoadFunctions(dummy);
			DestroyWindow(dummy);

			_cvis = true;
			isInit = true;

			UpdateMonitorInfo();

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

		if (isFullScr)
		{
			dwExStyle = 0;
			dwStyle = WS_POPUP;
		}
		else
		{
			dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
			dwStyle = WS_OVERLAPPEDWINDOW;
		}
		
		SetThreadDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

		RECT wr = {mX, mY, mX + mW, mY + mH};
		AdjustWindowRectEx(&wr, dwStyle, FALSE, dwExStyle);

		if (isFullScr)
			wr = monitors[0].rect;

		hWnd = CreateWindowEx(dwExStyle, wc_name, mName.c_str(),
			dwStyle, wr.left, wr.top,
			wr.right - wr.left, wr.bottom - wr.top,
			NULL, NULL,
			hInstance, NULL);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);
		ShowWindow(hWnd, SW_SHOW);
		SetFocus(hWnd);
		SetForegroundWindow(hWnd);
		pContext = std::shared_ptr<RenderContext>(new RenderContext(hWnd, z_depth));
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

	int GUIObject::GetWidth()
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		return rect.right - rect.left;
	}

	int GUIObject::GetHeight()
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		return rect.bottom - rect.top;
	}

	void GUIObject::SetCursorVisible(bool visible) { if (_cvis!=visible) ShowCursor(visible); _cvis = visible; }
	void GUIObject::SetCursorPosition(int x, int y) { SetCursorPos(x, y); }
	bool GUIObject::HasCursor(void) {
		//POINT point;
		//GetCursorPos(&point);
		//return point.x>=mX && point.y>=mY && point.x<=mX+mW && point.y<=mY+mH && GetFocus()==hWnd && GetForegroundWindow()==hWnd;
		return GetForegroundWindow() == hWnd;
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
		if (HasCursor())
		{
			if (locked)
			{
				RECT rect;
					GetClientRect(hWnd, &rect);
					ClientToScreen(hWnd, reinterpret_cast<POINT*>(&rect.left));
					ClientToScreen(hWnd, reinterpret_cast<POINT*>(&rect.right));
					ClipCursor(&rect);
			}
			else
			{
				ClipCursor(0);
			}
		}
		crsLock = locked;
	}
}