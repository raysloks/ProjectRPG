#ifndef SYNC_STATE_H
#define SYNC_STATE_H

#include "streams.h"
#include <functional>
#include <vector>
#include <stack>
#include <map>

class ClientData;

class SyncState
{
public:
	SyncState(void);
	~SyncState(void);

	size_t allocate(std::function<void(ClientData&)> f, std::function<bool(ClientData&)> c);
	void deallocate(size_t index);
	void update(size_t index);
	void set(size_t index, int value);
	void prep(const std::map<size_t, uint32_t>& map, ClientData& client_data);

	static void increment(int& i);
	//static void increment(uint32_t& ui32);
	static bool is_ordered(const int& first, const int& second);
	//static bool is_ordered(const uint32_t& first, const uint32_t& second);

	std::vector<int> sync;
	std::vector<std::function<void(ClientData&)>> func;
	std::vector<std::function<bool(ClientData&)>> condition;
	std::vector<bool> pass;
	size_t npass;
	std::stack<size_t> empty;
	std::map<size_t, int> conf;
};

#endif