#include "binostream.h"

namespace std
{
	binostream::binostream(streambuf *buf) : basic_ostream(buf)
	{
	}

	binostream& operator<<(binostream& os, const bool &b)
	{
		os.write((char*)&b, sizeof(b));
		return os;
	}

	binostream& operator<<(binostream& os, const unsigned char &uc)
	{
		os.write((char*)&uc, sizeof(uc));
		return os;
	}

	binostream& operator<<(binostream& os, const signed char &c)
	{
		os.write((char*)&c, sizeof(c));
		return os;
	}

	binostream& operator<<(binostream& os, const unsigned short &us)
	{
		os.write((char*)&us, sizeof(us));
		return os;
	}

	binostream& operator<<(binostream& os, const signed short &s)
	{
		os.write((char*)&s, sizeof(s));
		return os;
	}

	binostream& operator<<(binostream& os, const unsigned int &ui)
	{
		os.write((char*)&ui, sizeof(ui));
		return os;
	}

	binostream& operator<<(binostream& os, const signed int &i)
	{
		os.write((char*)&i, sizeof(i));
		return os;
	}

	binostream& operator<<(binostream& os, const float &f)
	{
		os.write((char*)&f, sizeof(f));
		return os;
	}

	binostream& operator<<(binostream& os, const std::string &str)
	{
		uint32_t size = str.size();
		os << size;
		os.write(str.c_str(), size);
		return os;
	}
}