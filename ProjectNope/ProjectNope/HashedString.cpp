#include "HashedString.h"
#include <unordered_map>

HashedString::HashedString(const std::string& str)
{
	strType = str;
	std::hash<std::string> h;
	iType = h(strType);
}

HashedString::~HashedString(void)
{
}

bool HashedString::operator==(const HashedString& rhs)const
{
	return iType==rhs.iType;
}