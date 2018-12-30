#pragma once

#include "Aura.h"

class ShieldAura :
	public Aura
{
public:
	ShieldAura();
	ShieldAura(instream& is, bool full);
	~ShieldAura();

	void attach(MobComponent * mob);
	void detach(MobComponent * mob);

	static AutoSerialFactory<ShieldAura, Aura> _factory;

	float part;
	int32_t amount;
};