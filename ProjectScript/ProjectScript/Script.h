#include "Token.h"
#include "ScriptCode.h"
#include "ScriptMemory.h"

#include <vector>

#ifndef SCRIPT_H
#define SCRIPT_H

class Script
{
public:
	Script(void);
	Script(std::istringstream& ss);
	~Script(void);

	void run(const std::shared_ptr<ScriptMemory> mem);

	std::shared_ptr<ScriptCode> func;
};

#endif