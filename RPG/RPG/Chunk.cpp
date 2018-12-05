#include "Chunk.h"

Chunk::Chunk()
{
}

Chunk::Chunk(Tile tile)
{
	for (size_t i = 0; i < CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE; ++i)
		tiles[i] = tile;
}

Chunk::~Chunk()
{
}

Tile Chunk::getTile(size_t x, size_t y, size_t z) const
{
	return tiles[x + (y << CHUNK_SHIFT) + (z << (CHUNK_SHIFT * 2))];
}

void Chunk::setTile(size_t x, size_t y, size_t z, Tile tile)
{
	tiles[x + (y << CHUNK_SHIFT) + (z << (CHUNK_SHIFT * 2))] = tile;
}