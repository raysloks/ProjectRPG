#pragma once

#include "ReferenceCounter.h"

template <class T>
class Reference
{
public:
	Reference(T * a, ReferenceCounter<T> * b)
	{
		obj = a;
		ref = b;
		ref->ref.insert(this);
	}

	Reference()
	{
		obj = nullptr;
		ref = nullptr;
	}

	Reference(const Reference<T>& rhs)
	{
		obj = rhs.obj;
		ref = rhs.ref;
		if (ref)
			ref->ref.insert(this);
	}

	~Reference()
	{
		if (ref)
			ref->ref.erase(this);
	}

	Reference<T>& operator=(const Reference<T>& rhs)
	{
		if (ref)
			ref->ref.erase(this);
		obj = rhs.obj;
		ref = rhs.ref;
		if (ref)
			ref->ref.insert(this);
		return *this;
	}

	T * obj;
	ReferenceCounter<T> * ref;
};