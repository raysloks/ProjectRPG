#ifndef BINISTREAM_H
#define BINISTREAM_H

#include <istream>

namespace std
{
	class binistream :
		public basic_istream<char, char_traits<char>>
	{
	public:
		binistream(streambuf *buf);
	};

	binistream& operator>>(binistream& is, bool& b);
	binistream& operator>>(binistream& is, int8_t& i8);
	binistream& operator>>(binistream& is, uint8_t& u8);
	binistream& operator>>(binistream& is, int16_t& i16);
	binistream& operator>>(binistream& is, uint16_t& u16);
	binistream& operator>>(binistream& is, int32_t& i32);
	binistream& operator>>(binistream& is, uint32_t& u32);
	binistream& operator>>(binistream& is, int64_t& i64);
	binistream& operator>>(binistream& is, uint64_t& u64);
	binistream& operator>>(binistream& is, float& f);
	binistream& operator>>(binistream& is, double& d);

	binistream& operator>>(binistream& is, std::string& str);
}

#endif