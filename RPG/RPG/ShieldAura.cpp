#include "ShieldAura.h"

AutoSerialFactory<ShieldAura, Aura> ShieldAura::_factory("ShieldAura");

#include <algorithm>

ShieldAura::ShieldAura(int32_t a)
{
	amount = a;
}

ShieldAura::ShieldAura(int32_t a, float d) : Aura(d)
{
	amount = a;
}

ShieldAura::ShieldAura(instream& is, bool full)
{
	is >> amount;
}

ShieldAura::~ShieldAura()
{
}

void ShieldAura::attach(MobComponent * mob)
{
	func.reset(new std::function<void(HitData&)>([this, mob](HitData& hit)
	{
		for (auto& dmg : hit.damage)
		{
			int32_t absorb = std::min(dmg.amount, amount);
			dmg.amount -= absorb;
			amount -= absorb;
		}
		if (amount <= 0)
			mob->remove_aura(this);
	}));
	mob->on_hit_taken.funcs.insert(std::make_pair(100, func));
}

void ShieldAura::readLog(instream& is)
{
	is >> amount;
}

void ShieldAura::writeLog(outstream& os)
{
	os << amount;
}

void ShieldAura::write_to(outstream& os)
{
	os << amount;
}

uint32_t ShieldAura::getSerial() const
{
	return _factory.id;
}
