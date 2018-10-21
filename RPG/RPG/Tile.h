#pragma once

#include <sys\types.h>

class Tile
{
public:
	Tile();
	~Tile();

	off_t solid;
};