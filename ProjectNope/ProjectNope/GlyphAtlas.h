#ifndef GLYPH_ATLAS_H
#define GLYPH_ATLAS_H

#include <memory>

class Texture;

class GlyphAtlas
{
public:
	GlyphAtlas(size_t width, size_t height);
	~GlyphAtlas();

	void insert(size_t glyph_width, size_t glyph_height, size_t * glyph_x, size_t * glyph_y);

	size_t w, h;
	std::shared_ptr<Texture> texture;

	size_t fill_x, fill_y, top_y;
};

#endif