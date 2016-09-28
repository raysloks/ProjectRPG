#ifndef COLLIDER_ITERATOR_H
#define COLLIDER_ITERATOR_H

#include "TypeIteratorBase.h"

#include <vector>

#include "ColliderComponent.h"

class ColliderIterator :
	public TypeIteratorBase
{
public:
	ColliderIterator(void);
	~ColliderIterator(void);

	void tick(float dTime);

	std::vector<ColliderComponent> target;
};

#endif