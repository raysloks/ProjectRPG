#ifndef LINK_H
#define LINK_H

#include <vector>

class Link
{
public:
	Link(void);
	~Link(void);

	std::vector<int> chunk;
	int lid;
};

#endif