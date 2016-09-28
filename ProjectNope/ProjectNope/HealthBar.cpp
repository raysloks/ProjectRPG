#include "HealthBar.h"

HealthBar::HealthBar(void)
{
}

HealthBar::HealthBar(int maxHealth)
{
	current = maxHealth;
	max = maxHealth;
}

HealthBar::~HealthBar(void)
{
}

outstream& operator<<(outstream& os, const HealthBar& hpbar)
{
	os << hpbar.current << hpbar.max;
	return os;
}

instream& operator>>(instream& is, HealthBar& hpbar)
{
	is >> hpbar.current >> hpbar.max;
	return is;
}