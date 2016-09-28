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
	};

}

#endif