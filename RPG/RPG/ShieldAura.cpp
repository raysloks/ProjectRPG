#include "ShieldAura.h"

AutoSerialFactory<ShieldAura, Aura> ShieldAura::_factory("ShieldAura");

ShieldAura::ShieldAura() : Aura(_factory.id)
{
}

ShieldAura::ShieldAura(instream& is, bool full) : Aura(_factory.id)
{
}

ShieldAura::~ShieldAura()
{
}

void ShieldAura::attach(MobComponent * mob)
{
}

void ShieldAura::detach(MobComponent * mob)
{
}
