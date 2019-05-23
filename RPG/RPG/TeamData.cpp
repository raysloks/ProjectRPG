#include "TeamData.h"

#include <cmath>

TeamData::TeamData()
{
}

TeamData::~TeamData()
{
}

void TeamData::clearProgress(void)
{
	progress.clear();
}

void TeamData::setProgress(MobComponent * mob, float new_progress)
{
	progress.resize(members.size());
	for (size_t i = 0; i < members.size(); i++)
	{
		if (members[i].second == mob)
		{
			progress[i] = std::fmaxf(progress[i], new_progress);
			break;
		}
	}
}
