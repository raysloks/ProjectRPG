#include "ColliderIterator.h"

ColliderIterator::ColliderIterator(void)
{
}

ColliderIterator::~ColliderIterator(void)
{
}

void ColliderIterator::tick(float dTime)
{
	for (auto i=target.begin();i!=target.end();++i)
	{
		for (auto j=i;j!=target.end();++j)
		{
			if (i->entity!=j->entity)
			{

			}
		}
	}
}