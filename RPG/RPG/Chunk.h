#pragma once

#include "Tile.h"

const size_t CHUNK_SHIFT = 6;
const size_t CHUNK_SIZE = 1 << CHUNK_SHIFT;

class Chunk
{
public:
	Chunk();
	Chunk(Tile tile);
	~Chunk();

	Tile getTile(size_t x, size_t y, size_t z) const;
	void setTile(size_t x, size_t y, size_t z, Tile tile);

	Tile tiles[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
};

