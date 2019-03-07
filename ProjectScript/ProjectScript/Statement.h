#ifndef STATEMENT_H
#define STATEMENT_H

#include "ScriptMemory.h"
#include "ScriptCompile.h"
#include "Token.h"
#include <vector>
#include <optional>

class ScriptCode;

class Statement
{
public:
	Statement(std::vector<std::shared_ptr<Statement>>& tokens, bool expression = false);
	Statement(std::shared_ptr<Statement> lhs, const Token& token, std::shared_ptr<Statement> rhs);
	Statement(int key);
	~Statement(void);

	void compile(ScriptCompile& comp);
	std::shared_ptr<Variable> run(const std::shared_ptr<ScriptMemory>& mem);

	bool hasReturn();
	bool isConstant(ScriptCompile& comp);
	ScriptTypeData getType(ScriptCompile& comp, bool operative);

	std::string output(const std::string& indent = "");

	std::shared_ptr<Statement> lhs, rhs;
	Token token;
	int keyword;
	std::shared_ptr<ScriptCode> code;
};

#endif