#ifndef FONT_RESOURCE_H
#define FONT_RESOURCE_H

#include "Resource.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <unordered_map>

class Glyph;

class FontResource :
	public Resource
{
public:
	FontResource(const std::string& buffer);
	~FontResource(void);

	FT_Face getFace(void);
	std::shared_ptr<Glyph> getGlyph(unsigned long code);

	std::string buffer;

	FT_Face ftFace;

	std::unordered_map<unsigned long, std::shared_ptr<Glyph>> glyphs;
};

#endif