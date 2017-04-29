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

	uint32_t addKnownUnit(uint32_t real_id);
	uint32_t forgetUnit(uint32_t real_id);
	uint32_t getUnit(uint32_t real_id) const;
	uint32_t getRealID(uint32_t client_side_id) const;

	void read(instream& is);
	void write(outstream& os);

	bool networked;
	bool loading;

	uint32_t client_id;
	std::vector<uint32_t> known_units;
	std::vector<uint32_t> per_entity_sync;
	std::vector<std::map<size_t, uint32_t>> sync;

	//TODO add stack for faster allocation
};

#endif