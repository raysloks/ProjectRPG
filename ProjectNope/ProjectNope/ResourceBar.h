#include "streams.h"

#ifndef RESOURCE_BAR_H
#define RESOURCE_BAR_H

class ResourceBar
{
public:
	ResourceBar(void);
	ResourceBar(int32_t maxAmount);
	~ResourceBar(void);

	int32_t current, max;
};

outstream& operator<<(outstream& os, const ResourceBar& bar);
instream& operator>>(instream& is, ResourceBar& bar);

#endif