#include <ostream>

#ifndef BINOSTREAM_H
#define BINOSTREAM_H

namespace std
{
	class binostream :
		public basic_ostream<char, char_traits<char>>
	{
	public:
		binostream(streambuf *buf);
	};

	binostream& operator<<(binostream& os, const bool &b);
	binostream& operator<<(binostream& os, const unsigned char &uc);
	binostream& operator<<(binostream& os, const signed char &c);
	binostream& operator<<(binostream& os, const unsigned short &us);
	binostream& operator<<(binostream& os, const signed short &s);
	binostream& operator<<(binostream& on, const unsigned int &ui);
	binostream& operator<<(binostream& os, const signed int &i);
	binostream& operator<<(binostream& os, const float &f);

	binostream& operator<<(binostream& os, const std::string &str);
}

#endif