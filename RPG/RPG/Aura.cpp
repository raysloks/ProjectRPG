#include "Aura.h"

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

#include "AuraFactory.h"
#include "AutoAuraFactory.h"

#include "ShieldAura.h"

std::vector<AuraFactory*> aura_factories;

void Aura::init()
{
	aura_factories.reserve(4096);

	aura_factories.resize(256);

	aura_factories[0] = new AutoAuraFactory<ShieldAura, 10, 20>();
}

Aura * Aura::create(uint32_t index)
{
	if (index < aura_factories.size())
	{
		Aura * aura = aura_factories[index]->create();
		aura->index = index;
		return aura;
	}
	return nullptr;
}

void Aura::release()
{
	for (auto factory : aura_factories)
		if (factory)
			delete factory;
	aura_factories.clear();
}