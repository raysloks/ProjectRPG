#pragma once

#include "SerializableRegistry.h"

class MobComponent;

class Aura
{
protected:
	Aura(uint32_t id);

public:
	virtual ~Aura();

	virtual void attach(MobComponent * mob);
	virtual void detach(MobComponent * mob);

	uint32_t _serial_id;

	static SerializableRegistry<Aura> _registry;
};