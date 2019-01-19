#pragma once

#include <vector>
#include <stack>

#include "Reference.h"
#include "ReferenceCounter.h"

template <class T>
class Manager
{
public:
	size_t addObj(T&& t)
	{
		if (alloc.empty())
		{
			if (obj.size() == obj.capacity())
			{
				size_t prev_size = obj.size();
				obj.emplace_back(std::move(t));
				ref.emplace_back(&obj[prev_size]);
				for (size_t i = 0; i < prev_size; i++)
				{
					ref[i].setRef(&obj[i]);
				}
			}
			else
			{
				obj.emplace_back(std::move(t));
				ref.emplace_back(&obj.back());
			}
			return obj.size() - 1;
		}
		else
		{
			size_t index = alloc.top();
			alloc.pop();
			obj.emplace(obj.cbegin() + index, std::move(t));
			return index;
		}
	}

	void delObj(size_t index)
	{
		ref[index].clear();
		alloc.push(index);
	}

	Reference<T> getRef(size_t index)
	{
		return ref[index].getRef();
	}

	std::vector<T> obj;
	std::vector<ReferenceCounter<T>> ref;
	std::stack<size_t> alloc;
};