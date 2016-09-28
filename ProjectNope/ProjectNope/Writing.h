#include <string>
#include "Vec2.h"

#ifndef WRITING_H
#define WRITING_H

namespace Writing
{
	void setFont(const std::string& name);
	void setColor(float r, float g, float b, float a = 1.0f);
	void setSize(const Vec2& size);
	void render(const std::string& text);

	std::string::iterator erase(std::string& str, const std::string::iterator& p);

	unsigned int getCodePoint(std::string::const_iterator& c, const std::string::const_iterator& end);

	size_t getLength(const std::string& text);
	std::pair<std::string::iterator, std::string::iterator> getRange(const std::string::iterator& begin, const std::string::iterator& pos, const std::string::iterator& end);
}

#endif