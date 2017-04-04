#include "binistream.h"

#include <string>
#include <iostream>

namespace std
{
	binistream::binistream(streambuf *buf) : basic_istream(buf)
	{
	}

	binistream& operator>>(binistream& is, bool& b)
	{
		is.read((char*)&b, sizeof(b));
		return is;
	}

	binistream& operator>>(binistream& is, int8_t& i8)
	{
		is.read((char*)&i8, sizeof(i8));
		return is;
	}

	binistream& operator>>(binistream& is, uint8_t& u8)
	{
		is.read((char*)&u8, sizeof(u8));
		return is;
	}

	binistream& operator>>(binistream& is, int16_t& i16)
	{
		is.read((char*)&i16, sizeof(i16));
		return is;
	}

	binistream& operator>>(binistream& is, uint16_t& u16)
	{
		is.read((char*)&u16, sizeof(u16));
		return is;
	}

	binistream& operator>>(binistream& is, int32_t& i32)
	{
		is.read((char*)&i32, sizeof(i32));
		return is;
	}

	binistream& operator>>(binistream& is, uint32_t& u32)
	{
		is.read((char*)&u32, sizeof(u32));
		return is;
	}


	binistream& operator>>(binistream& is, int64_t& i64)
	{
		is.read((char*)&i64, sizeof(i64));
		return is;
	}

	binistream& operator>>(binistream& is, uint64_t& u64)
	{
		is.read((char*)&u64, sizeof(u64));
		return is;
	}

	binistream& operator>>(binistream& is, float& f)
	{
		is.read((char*)&f, sizeof(f));
		return is;
	}

	binistream& operator>>(binistream& is, double& d)
	{
		is.read((char*)&d, sizeof(d));
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