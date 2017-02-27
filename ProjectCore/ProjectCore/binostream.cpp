#include "binostream.h"

namespace std
{
	binostream::binostream(streambuf *buf) : basic_ostream(buf)
	{
	}

	binostream& operator<<(binostream& os, const bool& b)
	{
		os.write((char*)&b, sizeof(b));
		return os;
	}

	binostream& operator<<(binostream& os, const int8_t& i8)
	{
		os.write((char*)&i8, sizeof(i8));
		return os;
	}

	binostream& operator<<(binostream& os, const uint8_t& u8)
	{
		os.write((char*)&u8, sizeof(u8));
		return os;
	}

	binostream& operator<<(binostream& os, const int16_t& i16)
	{
		os.write((char*)&i16, sizeof(i16));
		return os;
	}

	binostream& operator<<(binostream& os, const uint16_t& u16)
	{
		os.write((char*)&u16, sizeof(u16));
		return os;
	}

	binostream& operator<<(binostream& os, const int32_t& i32)
	{
		os.write((char*)&i32, sizeof(i32));
		return os;
	}

	binostream& operator<<(binostream& os, const uint32_t& u32)
	{
		os.write((char*)&u32, sizeof(u32));
		return os;
	}

	binostream& operator<<(binostream& os, const int64_t& i64)
	{
		os.write((char*)&i64, sizeof(i64));
		return os;
	}

	binostream& operator<<(binostream& os, const uint64_t& u64)
	{
		os.write((char*)&u64, sizeof(u64));
		return os;
	}

	binostream& operator<<(binostream& os, const float& f)
	{
		os.write((char*)&f, sizeof(f));
		return os;
	}

	binostream& operator<<(binostream& os, const std::string& str)
	{
		uint32_t size = (uint32_t)str.size();
		os << size;
		os.write(str.c_str(), size);
		return os;
	}
}