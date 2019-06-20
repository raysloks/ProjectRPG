#pragma once

#include "AbilityContext.h"

class Effect
{
public:
	virtual ~Effect() {}
	
	virtual void apply(const AbilityContext& ac) const = 0;
};