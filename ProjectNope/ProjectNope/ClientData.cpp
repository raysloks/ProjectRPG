#include "ClientData.h"
#include <iostream>

class Server;
class Client;
extern Server * server;
extern Client * client;

ClientData::ClientData(void)
{
	client_id = 0xffffffff;
	networked = true;
	loading = true;
}

ClientData::ClientData(instream& is)
{
	read(is);
	networked = true;
}

ClientData::~ClientData(void)
{
}

uint32_t ClientData::addKnownUnit(uint32_t id)
{
	for (size_t i = 0; i < known_units.size(); i++)
	{
		if (known_units[i] == 0xffffffff) {
			known_units[i] = id;
			++unit_uid[i];
			up_to_date[i] = false;
			return i;
		}
	}
	known_units.push_back(id);
	unit_uid.resize(known_units.size(), 0);
	up_to_date.resize(known_units.size(), false);
	per_entity_sync.resize(known_units.size(), 0);
	sync.resize(known_units.size());
	return known_units.size() - 1;
}

uint32_t ClientData::forgetUnit(uint32_t id)
{
	for (size_t i = 0; i < known_units.size(); ++i)
	{
		if (known_units[i] == id)
		{
			known_units[i] = 0xffffffff;
			sync[i].clear();
			return i;
		}
	}
	return 0xffffffff;
}

uint32_t ClientData::getUnit(uint32_t id) const
{
	if (!networked)
		return id;
	for (size_t i = 0; i < known_units.size(); i++)
	{
		if (known_units[i] == id) {
			return i;
		}
	}
	return 0xffffffff;
}

uint32_t ClientData::getRealID(uint32_t id) const
{
	if (id < known_units.size())
		return known_units[id];
	return 0xffffffff;
}

void ClientData::write(outstream& os)
{
	os << loading;
}

void ClientData::read(instream& is)
{
	is >> loading;
}