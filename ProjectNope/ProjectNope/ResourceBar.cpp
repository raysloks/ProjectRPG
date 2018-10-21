#include "ResourceBar.h"

ResourceBar::ResourceBar(void)
{
}

ResourceBar::ResourceBar(int32_t maxAmount)
{
	current = maxAmount;
	max = maxAmount;
}

ResourceBar::~ResourceBar(void)
{
}

int32_t ResourceBar::Add(int32_t amount)
{
	current += amount;
	int32_t overflow = current - max;
	current = max;
	return amount - overflow;
}

int32_t ResourceBar::Remove(int32_t amount)
{
	current -= amount;
	int32_t underflow = -current;
	current = 0;
	return amount - underflow;
}

outstream& operator<<(outstream& os, const ResourceBar& bar)
{
	os << bar.current << bar.max;
	return os;
}

instream& operator>>(instream& is, ResourceBar& bar)
{
	is >> bar.current >> bar.max;
	return is;
}