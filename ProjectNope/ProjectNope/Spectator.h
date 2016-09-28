#include "PNEntity.h"

#ifndef SPECTATOR_H
#define SPECTATOR_H

class Spectator :
	public PNEntity
{
public:
	Spectator(const GlobalPosition& pos);
	Spectator(instream& is, bool full);
	~Spectator(void);

	static const AutoSerialFactory<Spectator> _factory_Spectator;
};

#endif