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

	void update(uint32_t next_sync, const std::vector<T>& next)
	{
		uint32_t diff = next_sync - sync;
		if (diff < INT32_MAX && diff <= next.size())
			queue.insert(queue.end(), next.end() - diff, next.end());
		sync = next_sync;
	}

	bool consume(const T& value)
	{
		auto i = std::find(queue.begin(), queue.end(), value);
		if (i != queue.end())
		{
			queue.erase(i);
			return true;
		}
		return false;
	}

	void writeLog(outstream& os, ClientData& client)
	{
		os << sync;
	}
	void readLog(instream& is)
	{
		/*uint32_t next_sync;
		is >> next_sync;
		update(next_sync);*/
		is >> sync;
	}

	void writeLog(outstream& os)
	{
		os << sync + uint32_t(queue.size());
		os << uint32_t(queue.size());
		for (auto& obj : queue)
			os << obj;
	}
	void readLog(instream& is, ClientData& client)
	{
		uint32_t next_sync, next_size;
		is >> next_sync >> next_size;
		std::vector<T> next(next_size);
		for (size_t i = 0; i < next_size; ++i)
			is >> next[i];
		update(next_sync, next);
	}

	uint32_t sync;
	std::vector<T> queue;
};