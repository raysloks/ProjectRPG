#ifndef CHUNK_H
#define CHUNK_H

#include "LinkContainer.h"
#include <map>
#include <string>
#include <boost\thread.hpp>

class World;

const int chunk_size_half = 30;
const int chunk_size = chunk_size_half*2;

class Chunk
{
public:
	Chunk(World * world, const std::vector<int>& pos);
	Chunk(World * world, const std::vector<int>& pos, const std::string& fname);
	~Chunk(void);

	void sync(void);

	size_t add(int id);
	void remove(size_t id);

	void transfer(size_t id, Chunk * chunk);

	World * world;

	std::vector<int> pos;

	std::vector<int> units;
	std::stack<size_t> alloc;

	std::string fname;
	bool loading;

	LinkContainer link;

	void unload(void);
	void save(const std::string& fname);
};

#endif