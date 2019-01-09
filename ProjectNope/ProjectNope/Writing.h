#ifndef WRITING_H
#define WRITING_H

#include <string>
#include "Vec2.h"
#include "Vec3.h"
#include "Vec4.h"

class RenderSetup;

namespace Writing
{
	void setFont(const std::string& name);
	void setColor(const Vec3& c);
	void setColor(const Vec4& c);
	void setColor(float r, float g, float b, float a = 1.0f);
	void setSize(size_t size);
	void setSize(size_t x_size, size_t y_size);
	void setScale(float s);
	void setOffset(const Vec2& offset);
	void render(const std::string& text, RenderSetup& rs);

	std::string::iterator erase(std::string& str, const std::string::iterator& p);

	unsigned int getCodePoint(std::string::const_iterator& c, const std::string::const_iterator& end);

	size_t getLength(const std::string& text);
	std::pair<std::string::iterator, std::string::iterator> getRange(const std::string::iterator& begin, const std::string::iterator& pos, const std::string::iterator& end);
}

#endif