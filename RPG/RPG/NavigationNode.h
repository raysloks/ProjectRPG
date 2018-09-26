#pragma once

#include <vector>

#include "Vec2.h"

class NavigationNode
{
public:
	NavigationNode();
	~NavigationNode();

	void AddNeighbour(float cost, size_t neighbour);

	std::vector<std::pair<float, size_t>> neighbours;
	Vec2 p;
};