#pragma once

#include <vector>

#include "NavigationNode.h"

class Mesh;

class NavigationMesh
{
public:
	NavigationMesh();
	NavigationMesh(const Mesh& mesh);
	~NavigationMesh();

	size_t GetClosestNode(const Vec2& p) const;

	std::vector<Vec2> GetPath(const Vec2& start, const Vec2& end) const;

	std::vector<NavigationNode> nodes;
};