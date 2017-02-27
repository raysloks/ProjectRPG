#include "streams.h"

#ifndef HEALTH_BAR_H
#define HEALTH_BAR_H

class HealthBar
{
public:
	HealthBar(void);
	HealthBar(float maxHealth);
	~HealthBar(void);

	float current, max;
};

outstream& operator<<(outstream& os, const HealthBar& hpbar);
instream& operator>>(instream& is, HealthBar& hpbar);

#endif