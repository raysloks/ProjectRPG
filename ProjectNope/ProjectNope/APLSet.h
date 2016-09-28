#ifndef APLSET_H
#define APLSET_H

#include "APL.h"

class BlendSet
{
public:
	BlendSet(void);
	~BlendSet(void);

	void set(float weight);
	
	std::vector<std::shared_ptr<APLBlend>> apl;
};

class SpeedSet
{
public:
	SpeedSet(void);
	~SpeedSet(void);

	void set(float speed);
	
	std::vector<std::shared_ptr<APLSpeed>> apl;
};

#endif