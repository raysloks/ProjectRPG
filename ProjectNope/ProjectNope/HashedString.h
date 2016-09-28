#include <string>

#ifndef HASHED_STRING_H
#define HASHED_STRING_H

class HashedString
{
public:
	unsigned int iType;
	std::string strType;

	HashedString(const std::string& str);
	~HashedString(void);

	bool operator==(const HashedString& rhs)const;
};

#endif