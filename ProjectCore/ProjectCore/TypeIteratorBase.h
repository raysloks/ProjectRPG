#ifndef TYPE_ITERATOR_BASE_H
#define TYPE_ITERATOR_BASE_H

class TypeIteratorBase
{
public:
	TypeIteratorBase(void){}
	virtual ~TypeIteratorBase(void){}

	virtual void tick(float dTime) = 0;
};

#endif