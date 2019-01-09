#pragma once

#include "Aura.h"

class ShieldAura :
	public Aura
{
public:
	ShieldAura(int32_t amount);
	ShieldAura(int32_t amount, float duration);
	ShieldAura(instream& is, bool full);
	~ShieldAura();

	void attach(MobComponent * mob);

	void readLog(instream& is);
	void writeLog(outstream& os);

	void write_to(outstream& os);

	uint32_t getSerial() const;

	static AutoSerialFactory<ShieldAura, Aura> _factory;

	int32_t amount;

	std::shared_ptr<std::function<void(HitData&)>> func;
};