#ifndef RIGID_BODY_ITERATOR_H
#define RIGID_BODY_ITERATOR_H

#include "TypeIteratorBase.h"

#include <vector>

#include "RigidBodyComponent.h"

class RigidBodyIterator :
	public TypeIteratorBase
{
public:
	RigidBodyIterator(void);
	~RigidBodyIterator(void);

	void tick(float dTime);

	std::vector<RigidBodyComponent> target;
};

#endif