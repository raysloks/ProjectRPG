#ifndef UNIT_CAPABILITY_H
#define UNIT_CAPABILITY_H

class Unit;

class UnitCapability
{
public:
	UnitCapability(Unit * unit);
	virtual ~UnitCapability(void);

	virtual void tick(float dTime){};

	Unit * unit;
};

#endif