#include "Aura.h"

SerializableRegistry<Aura> Aura::_registry;

Aura::Aura() : duration(std::numeric_limits<float>::infinity())
{
}

Aura::Aura(float d) : duration(d)
{
}

Aura::~Aura()
{
}

void Aura::attach(MobComponent * mob)
{
}

void Aura::readLog(instream& is)
{
}

void Aura::writeLog(outstream& os)
{
}

void Aura::write_to(outstream& os)
{
}

uint32_t Aura::getSerial() const
{
	return 0;
}
