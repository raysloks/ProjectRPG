#ifndef BINISTREAM_H
#define BINISTREAM_H

#include <istream>

#include <tuple>
#include <vector>
#include <map>

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

	template <class T, typename = std::enable_if<std::is_trivial<T>::value>>
	binistream& operator>>(binistream& is, T& obj)
	{
		is.read((char*)&obj, sizeof(T));
		return is;
	}
	template <class T>
	binistream& operator>>(binistream& is, std::vector<T>& vec)
	{
		uint32_t size;
		is >> size;
		vec.resize(size);
		for (size_t i = 0; i < size; ++i)
		{
			is >> vec[i];
		}
		return is;
	}
	template <class Key, class Value>
	binistream& operator>>(binistream& is, std::map<Key, Value>& m)
	{
		uint32_t size;
		is >> size;
		m.clear();
		for (size_t i = 0; i < size; ++i)
		{
			Key key;
			Value value;
			is >> key >> value;
			m.insert(std::make_pair(key, value));
		}
		return is;
	}
}

#endif