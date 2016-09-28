#include "Resource.h"
#include <string>

#ifndef STRING_RESOURCE_H
#define STRING_RESOURCE_H

class StringResource
	: public Resource
{
public:
	StringResource(void);
	StringResource(const std::string& string);
	~StringResource(void);

	std::string string;
};

#endif