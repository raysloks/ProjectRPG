#ifndef PLAYER_SAVE_STATE_H
#define PLAYER_SAVE_STATE_H

#include "streams.h"
#include "GlobalPosition.h"

class PlayerSaveState
{
public:
	PlayerSaveState(void);
	PlayerSaveState(const PlayerSaveState& pss);
	~PlayerSaveState(void);

	std::string name, password;
	GlobalPosition save_pos;

	void save(void);
	void save_async(void);
};

outstream& operator<<(outstream& os, const PlayerSaveState& pss);
instream& operator>>(instream& is, PlayerSaveState& pss);

#endif