#include <sstream>

#ifndef TOKEN_H
#define TOKEN_H

class Token
{
public:
	Token(void);
	Token(std::istringstream& ss);
	~Token(void);

	std::string lexeme;
};

#endif