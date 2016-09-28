#include "APLSet.h"

BlendSet::BlendSet(void)
{
}

BlendSet::~BlendSet(void)
{
}

void BlendSet::set(float weight)
{
	for (auto i=apl.begin();i!=apl.end();++i)
		(*i)->weight = weight;
}

SpeedSet::SpeedSet(void)
{
}

SpeedSet::~SpeedSet(void)
{
}

void SpeedSet::set(float speed)
{
	for (auto i=apl.begin();i!=apl.end();++i)
		(*i)->speed = speed;
}