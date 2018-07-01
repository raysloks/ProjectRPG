#pragma once

#include <cstdint>

class Synchron
{
public:
	Synchron();
	~Synchron();

	uint32_t timestamp;

	bool isNewerThan(Synchron other);

	bool isOlderThan(Synchron other);
};