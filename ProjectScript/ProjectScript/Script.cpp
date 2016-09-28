#include "Script.h"

Script::Script(void)
{
}

#include <iostream>

Script::Script(std::istringstream& ss)
{
	std::vector<std::shared_ptr<Statement>> tokens;
	while (!ss.eof())
	{
		tokens.push_back(std::shared_ptr<Statement>(new Statement(0, Token(ss), 0)));
		if (tokens.back()->token.lexeme.size()==0)
			tokens.pop_back();
	}

	func.reset(new ScriptCode(tokens));
}

Script::~Script(void)
{
}

void Script::run(std::shared_ptr<ScriptMemory> mem)
{
	if (func!=0)
		func->run(mem);
}