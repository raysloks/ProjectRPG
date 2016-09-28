#include "PlayerSaveState.h"
#include <fstream>
#include <boost\thread.hpp>

PlayerSaveState::PlayerSaveState(void)
{
}

PlayerSaveState::PlayerSaveState(const PlayerSaveState& pss)
{
	name = pss.name;
	password = pss.password;

	save_pos = pss.save_pos;
}

PlayerSaveState::~PlayerSaveState(void)
{
}

void PlayerSaveState::save(void)
{
	std::string fname = std::string("save/") + name;
	std::fstream f;
	f.open(fname, std::ios::out | std::ios::binary);
	outstream os(f.rdbuf());
	os << *this;
	f.close();
}

void PlayerSaveState::save_async(void)
{
	std::shared_ptr<PlayerSaveState> pss(new PlayerSaveState(*this));
	boost::thread t([=] (void) {
		pss->save();
	});
}

outstream& operator<<(outstream& os, const PlayerSaveState& pss)
{
	os << pss.save_pos;
	return os;
}

instream& operator>>(instream& is, PlayerSaveState& pss)
{
	is >> pss.save_pos;
	return is;
}