#ifndef GUI_OBJECT_H
#define GUI_OBJECT_H

#include <memory>
#include <list>
#include <vector>
#include <iostream>

#include "KeyEvent.h"
#include "RenderContext.h"

#include <Windows.h>

#include <Xinput.h>

namespace Platform
{

	class GUIObject
	{
	public:
		GUIObject(const std::string& name, int x, int y, int w, int h, int depth_depth = 0, bool fullscreen = false);
		virtual ~GUIObject(void);

		void Tick(void);

		static void PollControllerState(void);
		static int pollFrequency;
		static XINPUT_STATE controllerState[4];

		static void SetCursorVisible(bool visible);
		static void SetCursorPosition(int x, int y);

		void SetCursorLock(bool locked);

		void Destroy(void);

		bool HasCursor(void);

		inline HWND getHWnd(){return hWnd;}

		inline std::shared_ptr<RenderContext> getContext(){return pContext;}

		inline int GetX(void){return mX;}
		inline int GetY(void){return mY;}
		inline int GetWidth(void){return mW;}
		inline int GetHeight(void){return mH;}
		inline bool IsFullscreen(void){return isFullScr;}
	protected:
		std::shared_ptr<RenderContext> pContext;
	private:
		virtual void OnResize(){}
		virtual void OnKeyDown(const KeyEvent& key){}
		virtual void OnKeyUp(const KeyEvent& key){}
		virtual void OnMouseMove(int x, int y, bool relative){}
		virtual void OnMouseWheel(int d){}
		virtual bool OnDestroy(){return false;}

		bool isFullScr;
		bool crsLock;
		HWND hWnd;
		std::string mName;
		int mX, mY, mW, mH, z_depth;
		void _InitWnd(void);
		static LRESULT CALLBACK WinProc(HWND hWnd,
			UINT Msg,
			WPARAM wParam,
			LPARAM lParam);
	};

}

#endif