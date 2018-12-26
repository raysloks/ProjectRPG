#pragma once

class MobComponent;

class Aura
{
protected:
	Aura();

public:
	virtual ~Aura();

	virtual void attach(MobComponent * mob);
	virtual void detach(MobComponent * mob);
};