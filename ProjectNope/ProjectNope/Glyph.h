#ifndef GLYPH_H
#define GLYPH_H

#include "FontResource.h"
#include FT_GLYPH_H

class RenderSetup;
class Mesh;
class Texture;
class GlyphAtlas;

class Glyph
{
public:
	Glyph(void);
	Glyph(FT_Face face, unsigned long code, GlyphAtlas * atlas);
	~Glyph(void);

	void insert(float * p, float * t, float offset_x, float offset_y) const;

	unsigned int index;
	FT_Glyph ftBitmap;

	float pos[12];
	float uv[12];
};

#endif