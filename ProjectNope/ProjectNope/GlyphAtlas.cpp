#include "GlyphAtlas.h"

#include "Texture.h"

GlyphAtlas::GlyphAtlas(size_t width, size_t height) : w(width), h(height)
{
	texture.reset(new Texture());

	glGenTextures(1, &texture->texid);
	glBindTexture(GL_TEXTURE_2D, texture->texid);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

	fill_x = 0;
	fill_y = 0;
	top_y = 0;
}

GlyphAtlas::~GlyphAtlas()
{
}

void GlyphAtlas::insert(size_t glyph_width, size_t glyph_height, size_t * glyph_x, size_t * glyph_y)
{
	if (fill_x + glyph_width >= w)
	{
		fill_x = 0;
		top_y = fill_y;
	}

	*glyph_x = fill_x;
	*glyph_y = top_y;

	fill_x += glyph_width;
	fill_y = max(fill_y, top_y + glyph_height);
}