#ifndef LINK_CONTAINER_H
#define LINK_CONTAINER_H

#include <vector>
#include <stack>

class LinkContainer
{
public:
	LinkContainer(void);
	~LinkContainer(void);
	
	size_t add(const std::pair<size_t, size_t>& ref);
	void remove(size_t lid);

	std::vector<std::pair<size_t, size_t>> oref;
	std::stack<size_t> alloc;
};

#endif