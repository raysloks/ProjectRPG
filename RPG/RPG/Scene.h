#pragma once

#include <vector>

#include "Tile.h"

class Scene
{
public:
	Scene(size_t width, size_t height);
	~Scene();

	Tile * getTile(size_t x, size_t y) const;

	size_t w, h;
	Tile * tiles;
};