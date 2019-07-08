#ifndef BINOSTREAM_H
#define BINOSTREAM_H

#include <ostream>

#include <vector>
#include <map>

namespace std
{
	class binostream :
		public basic_ostream<char, char_traits<char>>
	{
	public:
		binostream(streambuf *buf);
	};

	binostream& operator<<(binostream& os, const bool& b);
	binostream& operator<<(binostream& os, const int8_t& i8);
	binostream& operator<<(binostream& os, const uint8_t& u8);
	binostream& operator<<(binostream& os, const int16_t& i16);
	binostream& operator<<(binostream& os, const uint16_t& u16);
	binostream& operator<<(binostream& os, const int32_t& i32);
	binostream& operator<<(binostream& os, const uint32_t& u32);
	binostream& operator<<(binostream& os, const int64_t& i64);
	binostream& operator<<(binostream& os, const uint64_t& u64);
	binostream& operator<<(binostream& os, const float& f);
	binostream& operator<<(binostream& os, const double& d);

	binostream& operator<<(binostream& os, const std::string &str);

	template <class T, typename = std::enable_if<std::is_trivial<T>::value>>
	binostream& operator<<(binostream& os, const T& obj)
	{
		os.write((const char*)&obj, sizeof(T));
		return os;
	}
	template <class T>
	binostream& operator<<(binostream& os, const std::vector<T>& vec)
	{
		os << (uint32_t)vec.size();
		for (auto i : vec)
		{
			os << i;
		}
		return os;
	}
	template <class Key, class Value>
	binostream& operator<<(binostream& os, const std::map<Key, Value>& m)
	{
		os << (uint32_t)m.size();
		for (auto i : m)
		{
			os << i.first << i.second;
		}
		return os;
	}
}

#endif