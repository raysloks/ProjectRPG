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

std::vector<AuraFactory*> aura_factories;

void Aura::init()
{
	aura_factories.reserve(4096);

	aura_factories.resize(256);

	aura_factories[0] = new AutoAuraFactory<ShieldAura>();
}

Aura * Aura::create(uint32_t index)
{
	if (index < aura_factories.size())
		return aura_factories[index]->create();
	return nullptr;
}

void Aura::release()
{
	for (auto factory : aura_factories)
		if (factory)
			delete factory;
	aura_factories.clear();
}