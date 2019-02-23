#pragma once

#include <vector>

#include "Alias.h"

class NPC
{
public:
	NPC();
	~NPC();

	std::vector<Alias> aliases;
};