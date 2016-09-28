#ifndef SYNC_CONTAINER_H
#define SYNC_CONTAINER_H

#include "streams.h"

#include <memory>
#include <vector>
#include <set>
#include <stack>

#include "SyncState.h"

template <class T>
class SyncContainer
{
public:
	SyncContainer(void):ss(0){}
	SyncContainer(SyncState * sync_state):ss(sync_state)
	{
		cs = ss->allocate(0);
	}
	~SyncContainer(void)
	{
		if (ss!=0) // maybe bracket everything
			ss->deallocate(cs);
		for (auto i=sync.begin();i!=sync.end();++i)
			ss->deallocate(*i);
	}

	void setSyncState(SyncState * sync_state)
	{
		if (ss!=0) {
			ss->deallocate(cs);
			for (auto i=sync.begin();i!=sync.end();++i)
				ss->deallocate(*i);
		}
		ss = sync_state;
		if (sync_state!=0)
		{
			cs = sync_state->allocate(std::function<void(ClientData&)>(), std::function<bool(ClientData&)>());
			for (auto i=sync.begin();i!=sync.end();++i) {
				const int id = std::distance(sync.begin(), i);
				*i = sync_state->allocate([this, id] (ClientData&) { //TODO function for visibility check
					conf.insert(id);
				}, std::function<bool(ClientData&)>());
			}
		}
	}

	void writeLog(outstream& os)
	{
		os << (uint32_t)conf.size();
		for (auto i=conf.begin();i!=conf.end();++i)
		{
			os << (uint32_t)*i;
			auto p = items[*i];
			if (p!=0)
			{
				os << (unsigned char)1;
				p->writeLog(os);
			}
			else
			{
				os << (unsigned char)0;
			}
		}
		conf.clear();
	}

	void readLog(instream& is)
	{
		uint32_t size, index;
		unsigned char type;
		is >> size;
		for (size_t i=0;i<size;++i)
		{
			is >> index >> type;
			if (type!=0)
			{
				if (index>=items.size())
					items.resize(index+1);
				if (items[index]==0)
					items[index].reset(new T());
				items[index]->readLog(is);
			}
			else
			{
				if (index<items.size())
					items[index] = 0;
			}
		}
	}

	size_t add(const std::shared_ptr<T>& item)
	{
		if (alloc.size()) {
			size_t id = alloc.top();
			items[id] = item;
			alloc.pop();
			return id;
		} else {
			size_t id = items.size();
			items.push_back(item);
			sync.push_back(ss->allocate([this, id] (ClientData&) { //TODO function for visibility check
				conf.insert(id);
			}, std::function<bool(ClientData&)>()));
			return id;
		}
	}

	void remove(size_t index)
	{
		if (index<items.size())
			if (items[index]!=0)
			{
				items[index]=0;
				alloc.push(index);
			}
	}

	size_t transfer(size_t index, SyncContainer<T>& target)
	{
		size_t ret = target.add(items[index]);
		remove(index);
		return ret;
	}

	void update(size_t index)
	{
		ss->update(cs);
		ss->update(sync[index]);
	}

	SyncState * ss;

	size_t cs;

	std::set<size_t> conf;
	std::vector<std::shared_ptr<T>> items;
	std::vector<size_t> sync;
	std::stack<size_t> alloc;
};

#endif