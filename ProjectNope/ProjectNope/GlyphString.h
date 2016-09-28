#ifndef GLYPH_STRING_H
#define GLYPH_STRING_H

#include <string>

class Glyph;

class GlyphString
{
public:
	GlyphString(void);
	GlyphString(const std::string& string);
	~GlyphString(void);

	void render(void);

	std::string font, string;
};

#endif