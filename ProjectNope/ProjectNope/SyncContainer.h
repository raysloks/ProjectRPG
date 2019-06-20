#ifndef SYNC_CONTAINER_H
#define SYNC_CONTAINER_H

#include "streams.h"

#include <memory>
#include <vector>
#include <set>
#include <stack>

#include "SyncState.h"

template <typename T>
class has_readLog
{
	typedef char one;
	struct two { char x[2]; };

	template <typename C> static one test(decltype(&C::readLog));
	template <typename C> static two test(...);

public:
	enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

template <typename T>
class has_writeLog
{
	typedef char one;
	struct two { char x[2]; };

	template <typename C> static one test(decltype(&C::writeLog));
	template <typename C> static two test(...);

public:
	enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

template <class T>
class SyncContainer
{
public:
	SyncContainer(void) : ss(nullptr) {}
	SyncContainer(SyncState * sync_state) : ss(sync_state)
	{
		cs = ss->allocate(nullptr);
	}
	~SyncContainer(void)
	{
		if (ss != nullptr)
		{
			ss->deallocate(cs);
			for (auto i = sync.begin(); i != sync.end(); ++i)
				ss->deallocate(*i);
		}
	}

	void setSyncState(SyncState * sync_state)
	{
		if (ss != nullptr)
		{
			ss->deallocate(cs);
			for (auto i = sync.begin(); i != sync.end(); ++i)
				ss->deallocate(*i);
		}
		ss = sync_state;
		if (sync_state != nullptr)
		{
			cs = sync_state->allocate(std::function<void(ClientData&)>(), std::function<bool(ClientData&)>());
			for (auto i = sync.begin(); i != sync.end(); ++i)
			{
				const int id = std::distance(sync.begin(), i);
				*i = sync_state->allocate([this, id] (ClientData&) //TODO function for visibility check
				{
					conf.insert(id);
				}, std::function<bool(ClientData&)>());
			}
		}
	}

	void writeLog(outstream& os)
	{
		//if (conf.empty())
		//	return;
		os << (uint32_t)conf.size();
		for (auto i = conf.begin(); i != conf.end(); ++i)
		{
			os << (uint32_t)*i;
			auto p = items[*i];
			if (p)
			{
				os << (unsigned char)1;
				if constexpr (has_writeLog<T>::value)
					p->writeLog(os);
				else
					os << *p;
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
		for (size_t i = 0; i < size; i++)
		{
			is >> index >> type;
			if (type != 0)
			{
				if (index >= items.size())
					items.resize(index + 1);
				if (items[index] == 0)
					items[index] = std::make_shared<T>();
				if constexpr (has_readLog<T>::value)
					items[index]->readLog(is);
				else
					is >> *items[index];
			}
			else
			{
				if (index<items.size())
					items[index] = nullptr;
			}
		}
	}

	size_t add(const std::shared_ptr<T>& item)
	{
		if (alloc.size())
		{
			size_t id = alloc.top();
			items[id] = item;
			alloc.pop();
			return id;
		}
		else
		{
			size_t id = items.size();
			items.push_back(item);
			sync.push_back(ss->allocate([this, id] (ClientData&) //TODO function for visibility check
			{
				conf.insert(id);
			}, std::function<bool(ClientData&)>()));
			return id;
		}
	}

	void remove(size_t index)
	{
		if (index < items.size())
		{
			if (items[index] != nullptr)
			{
				items[index].reset();
				alloc.push(index);
			}
		}
	}

	std::shared_ptr<T> get(size_t index)
	{
		if (index < items.size())
			return items[index];
		return nullptr;
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