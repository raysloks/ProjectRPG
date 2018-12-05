#pragma once

#include <vector>

#include "Chunk.h"
#include "Mesh.h"

class Scene
{
public:
	Scene(size_t width, size_t height, size_t depth);
	~Scene();

	Tile getTile(size_t x, size_t y, size_t z) const;
	void setTile(size_t x, size_t y, size_t z, Tile tile);

	std::shared_ptr<Mesh> createMesh() const;

	size_t w, h, d;
	Chunk **chunks, *empty, *solid;
};