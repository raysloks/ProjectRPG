#include "ClientData.h"
#include <iostream>

class Server;
class Client;
extern Server * server;
extern Client * client;

ClientData::ClientData(void)
{
	unit_id = -1;
	client_id = -1;
	client_type = -1;
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

int ClientData::addKnownUnit(int id)
{
	for (int i=0;i<known_units.size();++i)
	{
		if (known_units[i]<0 || known_units[i]==id) {
			known_units[i] = id;
			return i;
		}
	}
	known_units.push_back(id);
	per_entity_sync.resize(known_units.size());
	sync.resize(known_units.size());
	return known_units.size()-1;
}

int ClientData::forgetUnit(int id)
{
	if (id==unit_id)
		unit_id = -1;
	for (int i=0;i<known_units.size();++i)
	{
		if (known_units[i]==id) {
			int ret = i;
			known_units[i] = -1;
			sync[i].clear();
			return ret;
		}
	}
	return -1;
}

int ClientData::getUnit(int id) const
{
	if (!networked)
		return id;
	for (int i=0;i<known_units.size();++i)
	{
		if (known_units[i]==id) {
			return i;
		}
	}
	return -1;
}

int ClientData::getRealID(int id) const
{
	if (id >= 0 && id < known_units.size())
		return known_units[id];
	return -1;
}

void ClientData::write(outstream& os)
{
	os << getUnit(unit_id) << loading;
}

void ClientData::read(instream& is)
{
	is >> unit_id >> loading;
}