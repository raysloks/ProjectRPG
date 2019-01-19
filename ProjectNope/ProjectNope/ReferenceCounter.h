#pragma once

#include <unordered_set>

template <class T>
class Reference;

template <class T>
class ReferenceCounter
{
public:
	ReferenceCounter()
	{
		obj = nullptr;
	}

	ReferenceCounter(T * a)
	{
		obj = a;
	}

	ReferenceCounter(ReferenceCounter<T>&& rhs)
	{
		obj = rhs.obj;
		ref = rhs.ref;
		for (auto r : ref)
		{
			r->ref = this;
		}
		rhs.ref.clear();
	}

	~ReferenceCounter()
	{
		for (auto r : ref)
		{
			r->obj = nullptr;
			r->ref = nullptr;
		}
	}

	Reference<T> getRef()
	{
		return Reference<T>(obj, this);
	}

	void setRef(T * a)
	{
		obj = a;
		for (auto r : ref)
		{
			r->obj = obj;
		}
	}

	void clear()
	{
		for (auto r : ref)
		{
			r->obj = nullptr;
			r->ref = nullptr;
		}
		ref.clear();
	}

	T * obj;
	std::unordered_multiset<Reference<T>*> ref;
};