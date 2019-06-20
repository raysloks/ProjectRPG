#pragma once

#include <vector>
#include <streams.h>

class ClientData;

template <class T>
class SyncQueue
{
public:
	void update(uint32_t next_sync)
	{
		uint32_t diff = next_sync - sync;
		if (diff < INT32_MAX && diff <= queue.size())
			queue.erase(queue.begin(), queue.begin() + diff);
		sync = next_sync;
	}

	void update(uint32_t next_sync, std::vector<T>& next)
	{
		uint32_t diff = next_sync - sync;
		if (diff < INT32_MAX && diff <= next.size())
			queue.insert(queue.end(), std::make_move_iterator(next.end() - diff), std::make_move_iterator(next.end()));
		sync = next_sync;
	}

	bool consumeValue(const T& value)
	{
		auto i = std::find(queue.begin(), queue.end(), value);
		if (i != queue.end())
		{
			queue.erase(i);
			return true;
		}
		return false;
	}

	void writeFromDestination(outstream& os)
	{
		os << sync;
	}

	void readFromDestination(instream& is)
	{
		uint32_t next_sync;
		is >> next_sync;
		update(next_sync);
		//is >> sync;
	}

	void writeFromSource(outstream& os)
	{
		os << sync + uint32_t(queue.size());
		os << uint32_t(queue.size());
		for (auto& obj : queue)
			os << obj;
	}

	void readFromSource(instream& is)
	{
		uint32_t next_sync, next_size;
		is >> next_sync >> next_size;
		std::vector<T> next(next_size);
		for (size_t i = 0; i < next_size; ++i)
			if constexpr (std::is_constructible<T, instream&>::value)
				next[i] = T(is);
			else
				is >> next[i];
		update(next_sync, next);
	}

	uint32_t sync;
	std::vector<T> queue;
};