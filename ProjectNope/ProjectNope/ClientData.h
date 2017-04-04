#ifndef CLIENT_DATA_H
#define CLIENT_DATA_H

#include "streams.h"
#include <vector>
#include <map>

class Syncable;

class ClientData
{
public:
	ClientData(void);
	ClientData(instream& is);
	~ClientData(void);

	int addKnownUnit(int real_id);
	int forgetUnit(int real_id);
	int getUnit(int real_id) const;
	int getRealID(int client_side_id) const;

	void read(instream& is);
	void write(outstream& os);

	bool networked;
	bool loading;

	int client_id;
	int unit_id;
	int client_type;
	std::vector<int> known_units;
	std::vector<int> per_entity_sync;
	std::vector<std::map<size_t, int>> sync;

	std::vector<Syncable*> meta_data;
	std::vector<std::map<size_t, int>> meta_sync;
	std::vector<bool> meta_new;
	//TODO add stack for faster allocation
};

#endif