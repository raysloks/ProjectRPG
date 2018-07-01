#include "Synchron.h"

Synchron::Synchron()
{
}

Synchron::~Synchron()
{
}

bool Synchron::isNewerThan(Synchron other)
{
	return timestamp - other.timestamp <= INT32_MAX;
}

bool Synchron::isOlderThan(Synchron other)
{
	return other.timestamp - timestamp <= INT32_MAX;
}