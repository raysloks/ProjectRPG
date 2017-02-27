#include "SerialHash.h"

uint32_t sdbm(const std::string& str)
{
	uint32_t hash = 0;
	for each (char c in str)
	{
		hash = c + (hash << 6) + (hash << 16) - hash;
	}
	return hash;
}