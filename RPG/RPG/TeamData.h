#ifndef TEAM_DATA_H
#define TEAM_DATA_H

#include <vector>

class MobComponent;

class TeamData
{
public:
	TeamData();
	~TeamData();

	void clearProgress(void);
	void setProgress(MobComponent * mob, float new_progress);

	size_t score;
	std::vector<std::pair<uint32_t, MobComponent*>> members;
	std::vector<float> progress;
};

#endif