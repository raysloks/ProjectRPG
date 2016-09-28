#ifndef ENTITY_LINK_CONTAINER_H
#define ENTITY_LINK_CONTAINER_H

#include "Link.h"
#include <vector>
#include <stack>

class EntityLinkContainer
{
public:
	EntityLinkContainer(void);
	~EntityLinkContainer(void);
	
	size_t add(const std::shared_ptr<Link>& ref);
	void remove(size_t lid);

	std::vector<std::shared_ptr<Link>> link;
	std::stack<size_t> alloc;
};

#endif