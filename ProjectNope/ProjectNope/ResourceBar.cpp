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