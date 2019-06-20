#pragma once

#include "AutoSerialFactory.h"
#include "SerializableRegistry.h"

#include "MobComponent.h"

class Aura
{
protected:
	Aura();
	Aura(float duration);

public:
	virtual ~Aura();

	virtual void attach(MobComponent * mob);

	virtual void readLog(instream& is);
	virtual void writeLog(outstream& os);

	virtual void write_to(outstream& os);

	virtual uint32_t getSerial() const;

	static SerializableRegistry<Aura> _registry;

	float duration;

	uint32_t icon;
};