#pragma once

#include "AbilityContext.h"

class Effect
{
public:
	virtual ~Effect() {}
	
	virtual void apply(const AbilityContext& ac) const = 0;

	static void init();
	static const Effect * get(uint32_t index);
	static void release();
};