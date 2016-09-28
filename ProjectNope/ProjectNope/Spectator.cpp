#include "Spectator.h"

const AutoSerialFactory<Spectator> Spectator::_factory_Spectator("Spectator");

Spectator::Spectator(const GlobalPosition& pos) : PNEntity(pos), Serializable(_factory_Spectator.id)
{
}

Spectator::~Spectator(void)
{
}

Spectator::Spectator(instream& is, bool full) : PNEntity(is, full), Serializable(_factory_Spectator.id)
{
}