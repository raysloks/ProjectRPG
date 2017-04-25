#ifndef GLYPH_H
#define GLYPH_H

#include "FontResource.h"
#include FT_GLYPH_H

class RenderSetup;
class Mesh;
class Texture;

class Glyph
{
public:
	Glyph(void);
	Glyph(FT_Face face, unsigned long code);
	~Glyph(void);

	void render(RenderSetup& rs);

	unsigned int index;
	FT_Glyph ftBitmap;

	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Texture> texture;
};

#endif