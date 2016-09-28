#include "ScriptCode.h"
#include "Script.h"

#include <iostream>

ScriptCode::ScriptCode(std::istringstream & ss)
{
	std::vector<std::shared_ptr<Statement>> tokens;
	while (!ss.eof())
	{
		tokens.push_back(std::shared_ptr<Statement>(new Statement(0, Token(ss), 0)));
		if (tokens.back()->token.lexeme.size() == 0)
			tokens.pop_back();
	}

	while (tokens.size())
	{
		statements.push_back(Statement(tokens));
	}
}

ScriptCode::ScriptCode(std::vector<std::shared_ptr<Statement>> tokens)
{
	while (tokens.size())
	{
		statements.push_back(Statement(tokens));
		//std::cout << statements.back().output("") << std::endl;
	}
}

ScriptCode::~ScriptCode(void)
{
}

std::shared_ptr<Variable> ScriptCode::run(std::shared_ptr<ScriptMemory> mem)
{
	for (auto i = statements.begin();i!=statements.end();++i) {
		i->run(mem);
		if (mem->return_var!=0)
			return mem->return_var;
	}
	return 0;
}