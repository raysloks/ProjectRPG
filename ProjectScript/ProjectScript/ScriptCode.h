#ifndef SCRIPT_CODE_H
#define SCRIPT_CODE_H

#include <vector>
#include "Statement.h"

class Script;

class ScriptCode
{
public:
	ScriptCode(std::istringstream& ss);
	ScriptCode(std::vector<std::shared_ptr<Statement>> tokens);
	~ScriptCode(void);

	std::shared_ptr<Variable> run(std::shared_ptr<ScriptMemory> mem);

	std::vector<Statement> statements;
};

#endif