#pragma once

#include "Aura.h"

class ShieldAura :
	public Aura
{
public:
	ShieldAura(int32_t amount);
	ShieldAura(int32_t amount, float duration);
	~ShieldAura();

	void attach(MobComponent * mob);

	void readLog(instream& is);
	void writeLog(outstream& os);

	int32_t amount;

	std::shared_ptr<std::function<void(HitData&)>> func;
};