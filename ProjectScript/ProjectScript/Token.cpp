#include "Token.h"

const std::string control = "()[]{};,";
const std::string ops = "|<>!&.=+-*/";
const std::string postops = "|=&";

#include <iostream>

Token::Token(void)
{
}

Token::Token(std::istringstream& ss)
{
	int type = -1;
	char ssc;
	while (true)
	{
		char c;
		ss.get(c);
		if (ss.eof())
			break;
		if (type==-1)
		{
			if (!isspace(c))
			{
				lexeme.append(1, c);
				if (isalpha((unsigned char)c) || c=='_')
					type = 0;
				if (isdigit((unsigned char)c))
					type = 1;
				if (c=='"' || c=='\'') {
					type = 2;
					ssc = c;
				}
				if (ops.find(c)!=std::string::npos)
					type = 4;
				if (control.find(c)!=std::string::npos)
					break;
			}
		}
		else
		{
			if (type==0)
			{
				if (isalpha((unsigned char)c) || isdigit((unsigned char)c) || c=='_') {
					lexeme.append(1, c);
				} else {
					ss.unget();
					break;
				}
			}
			if (type==1)
			{
				if (isdigit((unsigned char)c) || c=='.') {
					lexeme.append(1, c);
				} else {
					ss.unget();
					break;
				}
			}
			if (type==2)
			{
				if (c=='\\') {
					type = 3;
				} else {
					if (c!=ssc)
						lexeme.append(1, c);
					else
						break;
				}
			}
			if (type==3)
			{
				lexeme.append(1, c);
				type = 2;
			}
			if (type==4)
			{
				if (postops.find(c)!=std::string::npos)
					lexeme.append(1, c);
				else
					ss.unget();
				break;
			}
		}
	}
}

Token::~Token(void)
{
}