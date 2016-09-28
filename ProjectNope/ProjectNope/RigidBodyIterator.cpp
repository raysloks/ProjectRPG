#include "RigidBodyIterator.h"

RigidBodyIterator::RigidBodyIterator(void)
{
}

RigidBodyIterator::~RigidBodyIterator(void)
{
}

void RigidBodyIterator::tick(float dTime)
{
	for (auto i=target.begin();i!=target.end();++i)
	{
		for (auto j=i;j!=target.end();++j)
		{
		}
	}
}