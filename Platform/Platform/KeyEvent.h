#include <Windows.h>
#include <string>

#ifndef KEYEVENT_H
#define KEYEVENT_H

namespace Platform
{

	class KeyEvent
	{
	public:
		KeyEvent(unsigned short key);
		KeyEvent(int key);
		~KeyEvent(void);

		unsigned short code;
		std::string chr;

		static const unsigned int A = 0x0000;
		static const unsigned int B = 0x0001;
		static const unsigned int C = 0x0002;
		static const unsigned int D = 0x0003;
		static const unsigned int E = 0x0004;
		static const unsigned int F = 0x0005;
		static const unsigned int G = 0x0006;
		static const unsigned int H = 0x0007;
		static const unsigned int I = 0x0008;
		static const unsigned int J = 0x0009;
		static const unsigned int K = 0x000a;
		static const unsigned int L = 0x000b;
		static const unsigned int M = 0x000c;
		static const unsigned int N = 0x000d;
		static const unsigned int O = 0x000e;
		static const unsigned int P = 0x000f;
		static const unsigned int Q = 0x0010;
		static const unsigned int R = 0x0011;
		static const unsigned int S = 0x0012;
		static const unsigned int T = 0x0013;
		static const unsigned int U = 0x0014;
		static const unsigned int V = 0x0015;
		static const unsigned int W = 0x0016;
		static const unsigned int X = 0x0017;
		static const unsigned int Y = 0x0018;
		static const unsigned int Z = 0x0019;
		static const unsigned int SPACE = 0x001a;
		static const unsigned int ESC = 0x001b;
		static const unsigned int TAB = 0x001c;
		static const unsigned int LSHIFT = 0x001d;
		static const unsigned int RSHIFT = 0x001e;
		static const unsigned int LCTRL = 0x001f;
		static const unsigned int RCTRL = 0x0020;
		static const unsigned int LMB = 0x0021;
		static const unsigned int MMB = 0x0022;
		static const unsigned int RMB = 0x0023;
		static const unsigned int DEL = 0x0024;
		static const unsigned int LEFT = 0x0025;
		static const unsigned int UP = 0x0026;
		static const unsigned int RIGHT = 0x0027;
		static const unsigned int DOWN = 0x0028;
		static const unsigned int ENTER = 0x0029;

		static const unsigned int F1 = 0x0031;
		static const unsigned int F2 = 0x0032;
		static const unsigned int F3 = 0x0033;
		static const unsigned int F4 = 0x0034;
		static const unsigned int F5 = 0x0035;
		static const unsigned int F6 = 0x0036;
		static const unsigned int F7 = 0x0037;
		static const unsigned int F8 = 0x0038;
		static const unsigned int F9 = 0x0039;
		static const unsigned int F10 = 0x003a;
		static const unsigned int F11 = 0x003b;
		static const unsigned int F12 = 0x003c;
		static const unsigned int F13 = 0x003d;
		static const unsigned int F14 = 0x003e;
		static const unsigned int F15 = 0x003f;
		static const unsigned int F16 = 0x0040;
		static const unsigned int F17 = 0x0041;
		static const unsigned int F18 = 0x0042;
		static const unsigned int F19 = 0x0043;
		static const unsigned int F20 = 0x0044;
		static const unsigned int F21 = 0x0045;
		static const unsigned int F22 = 0x0046;
		static const unsigned int F23 = 0x0047;
		static const unsigned int F24 = 0x0048;

		static const unsigned int N0 = 0x0050;
		static const unsigned int N1 = 0x0051;
		static const unsigned int N2 = 0x0052;
		static const unsigned int N3 = 0x0053;
		static const unsigned int N4 = 0x0054;
		static const unsigned int N5 = 0x0055;
		static const unsigned int N6 = 0x0056;
		static const unsigned int N7 = 0x0057;
		static const unsigned int N8 = 0x0058;
		static const unsigned int N9 = 0x0059;
	};

}

#endif