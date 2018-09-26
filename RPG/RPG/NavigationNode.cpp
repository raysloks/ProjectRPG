#include "NavigationNode.h"

NavigationNode::NavigationNode()
{
}

NavigationNode::~NavigationNode()
{
}

void NavigationNode::AddNeighbour(float cost, size_t neighbour)
{
	for (auto n : neighbours)
	{
		if (n.second == neighbour)
			return;
	}
	neighbours.push_back(std::make_pair(cost, neighbour));
}