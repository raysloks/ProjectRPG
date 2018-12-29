#include "Aura.h"

SerializableRegistry<Aura> Aura::_registry;

Aura::Aura(uint32_t id) : _serial_id(id)
{
}

Aura::~Aura()
{
}

void Aura::attach(MobComponent * mob)
{
}

void Aura::detach(MobComponent * mob)
{
}