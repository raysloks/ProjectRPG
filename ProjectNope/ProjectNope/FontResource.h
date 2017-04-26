#ifndef FONT_RESOURCE_H
#define FONT_RESOURCE_H

#include "Resource.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <map>
#include <unordered_map>

class GlyphAtlas;
class Glyph;

class FontResource :
	public Resource
{
public:
	FontResource(const std::string& buffer);
	~FontResource(void);

	FT_Face getFace(size_t x_size, size_t y_size);
	std::shared_ptr<Glyph> getGlyph(unsigned long code, size_t x_size, size_t y_size);

	std::string buffer;

	FT_Face ftFace;

	std::map<std::pair<unsigned long, std::pair<size_t, size_t>>, std::shared_ptr<Glyph>> glyphs;
	std::shared_ptr<GlyphAtlas> atlas;
};

#endif