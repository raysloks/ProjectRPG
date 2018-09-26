#include "NavigationMesh.h"

#include <set>

#include "Mesh.h"

NavigationMesh::NavigationMesh()
{
}

NavigationMesh::NavigationMesh(const Mesh& mesh)
{
	nodes.resize(mesh.vert.size());
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		nodes[i].p = mesh.vert[i].p;
	}
	for (auto set : mesh.sets)
	{
		for (auto face : set.vertices)
		{
			float ab = (nodes[face.a].p - nodes[face.b].p).Len();
			float bc = (nodes[face.b].p - nodes[face.c].p).Len();
			float ca = (nodes[face.c].p - nodes[face.a].p).Len();

			nodes[face.a].AddNeighbour(ab, face.b);
			nodes[face.a].AddNeighbour(ca, face.c);

			nodes[face.b].AddNeighbour(ab, face.a);
			nodes[face.b].AddNeighbour(bc, face.c);

			nodes[face.c].AddNeighbour(bc, face.b);
			nodes[face.c].AddNeighbour(ca, face.a);
		}
	}
}

NavigationMesh::~NavigationMesh()
{
}

size_t NavigationMesh::GetClosestNode(const Vec2& p) const
{
	size_t closest = 0;
	float distance = INFINITY;
	for (size_t i = 0; i < nodes.size(); ++i)
	{
		float l = (nodes[i].p - p).LenPwr();
		if (l < distance)
		{
			distance = l;
			closest = i;
		}
	}
	return closest;
}

#include <iostream>

std::vector<Vec2> NavigationMesh::GetPath(const Vec2& start, const Vec2& end) const
{
	std::set<size_t> visited;

	std::vector<size_t> origin(nodes.size());
	std::vector<float> score(nodes.size(), INFINITY);
	std::vector<float> score_total(nodes.size(), INFINITY);

	size_t start_node = GetClosestNode(start);
	size_t end_node = GetClosestNode(end);

	std::cout << start_node << " " << end_node << std::endl;

	score[start_node] = 0.0f;
	score_total[start_node] = (nodes[end_node].p - nodes[start_node].p).Len();

	std::set<size_t> open;
	
	open.insert(start_node);

	while (!open.empty())
	{
		auto i = open.begin();
		size_t node = *i;
		for (++i; i != open.end(); ++i)
		{
			if (score_total[*i] < score_total[node])
				node = *i;
		}

		if (node == end_node)
			break;

		open.erase(node);
		visited.insert(node);

		for (auto neighbour : nodes[node].neighbours)
		{
			if (visited.find(neighbour.second) == visited.end())
			{
				float neighbour_score = score[node] + neighbour.first;

				auto open_neighbour = open.find(neighbour.second);
				if (open_neighbour == open.end())
					open.insert(neighbour.second);
				else
					if (neighbour_score >= score[*open_neighbour])
						continue;

				origin[neighbour.second] = node;
				score[neighbour.second] = neighbour_score;
				score_total[neighbour.second] = neighbour_score + (nodes[end_node].p - nodes[neighbour.second].p).Len();
			}
		}
	}

	std::vector<Vec2> path;

	if (score_total[end_node] == INFINITY)
		return path;

	size_t current = end_node;
	while (current != start_node)
	{
		std::cout << current << std::endl;
		path.push_back(nodes[current].p);
		current = origin[current];
	}

	return path;
}