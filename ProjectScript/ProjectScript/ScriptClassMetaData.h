#pragma once

#include "ScriptBaseClassMetaData.h"

#include <vector>
#include <map>
#include <string>

template <class T>
class ScriptClassMetaData : ScriptBaseClassMetaData
{
public:
	ScriptClassMetaData()
	{
		vars = T.meta.vars;
		size = sizeof(T);
	}
	~ScriptClassMetaData() {}

	std::vector<unsigned char> code;
	unsigned int size;
};

