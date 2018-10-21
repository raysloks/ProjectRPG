#include "Scene.h"

Scene::Scene(size_t width, size_t height) : w(width), h(height)
{
	tiles = new Tile[w * h];
}

Scene::~Scene()
{
	delete[] tiles;
}

Tile * Scene::getTile(size_t x, size_t y) const
{
	return tiles + x + y * w;
}
