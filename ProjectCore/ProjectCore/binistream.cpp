#include "binistream.h"

#include <string>
#include <iostream>

namespace std
{
	binistream::binistream(streambuf *buf) : basic_istream(buf)
	{
	}

	binistream& operator>>(binistream& is, bool &b)
	{
		is.read((char*)&b, sizeof(b));
		return is;
	}

	binistream& operator>>(binistream& is, unsigned char &uc)
	{
		is.read((char*)&uc, sizeof(uc));
		return is;
	}

	binistream& operator>>(binistream& is, signed char &c)
	{
		is.read((char*)&c, sizeof(c));
		return is;
	}

	binistream& operator>>(binistream& is, unsigned short &us)
	{
		is.read((char*)&us, sizeof(us));
		return is;
	}

	binistream& operator>>(binistream& is, signed short &s)
	{
		is.read((char*)&s, sizeof(s));
		return is;
	}

	binistream& operator>>(binistream& is, unsigned int &ui)
	{
		is.read((char*)&ui, sizeof(ui));
		return is;
	}

	binistream& operator>>(binistream& is, signed int &i)
	{
		is.read((char*)&i, sizeof(i));
		return is;
	}

	binistream& operator>>(binistream& is, float &f)
	{
		is.read((char*)&f, sizeof(f));
		return is;
	}
	
	binistream& operator>>(binistream& is, std::string &str)
	{
		uint32_t size;
		is.read((char*)&size, sizeof(size));
		//std::cout << size;
		str.resize(size);
		is.read(&str[0], size);
		//std::cout << " " << str << std::endl;
		return is;
	}
}