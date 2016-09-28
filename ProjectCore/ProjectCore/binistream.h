#include <istream>

#ifndef BINISTREAM_H
#define BINISTREAM_H

namespace std
{
	class binistream :
		public basic_istream<char, char_traits<char>>
	{
	public:
		binistream(streambuf *buf);
	};

	binistream& operator>>(binistream& is, bool &b);
	binistream& operator>>(binistream& is, unsigned char &uc);
	binistream& operator>>(binistream& is, signed char &c);
	binistream& operator>>(binistream& is, unsigned short &us);
	binistream& operator>>(binistream& is, signed short &s);
	binistream& operator>>(binistream& in, unsigned int &ui);
	binistream& operator>>(binistream& is, signed int &i);
	binistream& operator>>(binistream& is, float &f);

	binistream& operator>>(binistream& is, std::string &str);
}

#endif