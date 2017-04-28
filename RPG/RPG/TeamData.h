#ifndef TEAM_DATA_H
#define TEAM_DATA_H

#include <vector>

class MobComponent;

class TeamData
{
public:
	TeamData();
	~TeamData();

	size_t score;
	std::vector<std::pair<uint32_t, MobComponent*>> members;
};

#endif