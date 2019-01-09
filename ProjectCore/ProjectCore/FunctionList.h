#pragma once

#include <map>
#include <functional>

template <class... Args>
class FunctionList
{
public:
	void add(size_t key, const std::shared_ptr<std::function<void(Args...)>>& func)
	{
		funcs.insert(std::make_pair(key, std::weak_ptr<std::function<void(Args...)>>(func)));
	}

	void call(Args... args)
	{
		for (auto i = funcs.begin(); i != funcs.end();)
		{
			if (auto observe = i->second.lock())
			{
				(*observe)(args...);
				++i;
			}
			else
			{
				i = funcs.erase(i);
			}
		}
	}

	std::multimap<size_t, std::weak_ptr<std::function<void(Args...)>>> funcs;
};