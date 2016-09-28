#ifndef SPAWNER_H
#define SPAWNER_H

#include "PNEntity.h"

class Spawner :
	public PNEntity
{
public:
	Spawner(const GlobalPosition& pos);
	Spawner(instream& is, bool full);
	~Spawner(void);

	static const AutoSerialFactory<Spawner> _factory_Spawner;

	virtual void tick(float dTime);

	bool has_spawned;
};

#endif