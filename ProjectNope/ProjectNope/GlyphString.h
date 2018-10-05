#ifndef GLYPH_STRING_H
#define GLYPH_STRING_H

#include <string>
#include <Vec2.h>

class RenderSetup;

class GlyphString
{
public:
	GlyphString(void);
	GlyphString(const std::string& string);
	~GlyphString(void);

	void render(RenderSetup& rs);

	std::string font, string;
	size_t x_size, y_size;
	Vec2 offset;
};

#endif