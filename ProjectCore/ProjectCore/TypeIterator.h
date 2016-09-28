#ifndef TYPE_ITERATOR_H
#define TYPE_ITERATOR_H

#include "TypeIteratorBase.h"

#include <vector>

template <class T>
class TypeIterator :
	public TypeIteratorBase
{
public:
	TypeIterator(void){}
	~TypeIterator(void){}

	void tick(float dTime)
	{
		for (auto i=target.begin();i!=target.end();++i)
			i->tick(dTime);
	}

	std::vector<T> target;
};

#endif