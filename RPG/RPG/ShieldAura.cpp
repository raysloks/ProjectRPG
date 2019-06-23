#include "ShieldAura.h"

#include <algorithm>

ShieldAura::ShieldAura(int32_t a)
{
	amount = a;
}

ShieldAura::ShieldAura(int32_t a, float d) : Aura(d)
{
	amount = a;
}

ShieldAura::~ShieldAura()
{
}

void ShieldAura::attach(MobComponent * mob)
{
	func = std::make_shared<std::function<void(HitData&)>>([this, mob](HitData& hit)
	{
		for (auto& dmg : hit.damage)
		{
			int32_t absorb = std::min(dmg.amount, amount);
			dmg.amount -= absorb;
			amount -= absorb;
		}
		if (amount <= 0)
			mob->remove_aura(this);
	});
	mob->on_hit_taken.funcs.insert(std::make_pair(100, func));
}

void ShieldAura::readLog(instream& is)
{
	is >> amount >> duration;
}

void ShieldAura::writeLog(outstream& os)
{
	os << amount << duration;
}