#pragma once

#include "MobComponent.h"

class Aura
{
protected:
	Aura();
	Aura(float duration);

public:
	virtual ~Aura();
	
	static void init();
	static Aura * create(uint32_t index);
	static void release();

	virtual void attach(MobComponent * mob);

	virtual void readLog(instream& is);
	virtual void writeLog(outstream& os);

	uint32_t index;

	float duration;
};